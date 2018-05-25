#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

char *readJSONFile() {
	FILE *fp = fopen("/home/u21700648/project/jsmn/data.json", "r"); // open "data.json"
	char oneLine[255] = "";
	char *STRING;
	STRING = (char *)malloc(sizeof(oneLine));
	int length = 0;

	while(1) {
		fgets(oneLine, sizeof(oneLine), fp); // get the file line by line
		if(feof(fp)) break;
		length += strlen(oneLine); // entire size of the file
		STRING = realloc(STRING, length + 1); // reallocate for the longer length of the string
		strcat(STRING, oneLine); // make the file a string
	}

	puts(STRING);
	fclose(fp);
	return STRING;
}

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex) {
	int i, count = 0;
	for(i = 0; i < tokcount; i++) {
		if((t+i)->type == JSMN_STRING && (t+i)->size == 1) {
			nameTokIndex[count] = i;
			count++;
		}
	}
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex) {
	printf("***** Name List *******\n");

	int count = 0, num = 0;
	while(nameTokIndex[count] != '\0') {
		num = nameTokIndex[count];
		printf("[NAME%2d] %.*s\n", (count + 1), (t+num)->end-(t+num)->start, jsonstr+(t+num)->start);
		count++;
	}
}

void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex) {
	int no = 0, num = 0;
	while(1) {
		printf("\nSelect Name's no (exit:0) >> ");
		scanf("%d", &no);
		if(no == 0) break;
		num = nameTokIndex[no-1];
		printf("[NAME%2d] %.*s\n", no, (t+num)->end-(t+num)->start, jsonstr+(t+num)->start);
		printf("%.*s\n", (t+num+1)->end-(t+num+1)->start, jsonstr+(t+num+1)->start);
	}
}

static const char *JSON_STRING;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	int nameTokIndex[100]; // for index of tokens that point name
	for (i = 0; i < 100; i++)
		nameTokIndex[i] = '\0';

	JSON_STRING = readJSONFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
//	if (r < 1 || t[0].type != JSMN_OBJECT) {
//		printf("Object expected\n");
//		return 1;
//	}

	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printNameList(JSON_STRING, t, nameTokIndex);
	selectNameList(JSON_STRING, t, nameTokIndex);

	return EXIT_SUCCESS;


	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		} //else {
			//printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
			//		JSON_STRING + t[i].start);
		//}
	}
	return EXIT_SUCCESS;
}
