#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

/* read the JSON file */
char *readJSONFile() {
	char fileName[20] = "", address[50] = "/home/u21700648/project/jsmn/";
	printf("원하는 파일명 입력 :");
	gets(fileName);
	strcat(address, fileName);
	strcat(address, ".json");
	FILE *fp = fopen(address, "r"); // open "data.json"
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

/* put names from the JSON file into a list */
void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex) {
	int i, count = 0, parent = 0;
	for(i = 0; i < tokcount; i++) {
		//printf("token%2d size: %d type: %d\n parent: %d", i, (t+i)->size, (t+i)->type, (t+i)->parent);
		if((t+i-1)->parent == 0) parent = (t+i)->parent;
		if((t+i)->type == JSMN_STRING && (t+i)->size == 1) { // if the token means name
			if((t->type == JSMN_OBJECT && (t+i)->parent == 0) || (t->type == JSMN_ARRAY && (t+i)->parent == parent)) {
				nameTokIndex[count] = i; // store the token number in the nameTokIndex array
				count++;
			}
		}
	}
}

/* print the list of names */
void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex) {
	printf("***** Name List *******\n");

	int count = 0, num = 0;
	while(nameTokIndex[count] != '\0') {
		num = nameTokIndex[count];
		printf("[NAME%2d] %.*s\n", (count + 1), (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the name
		count++;
	}
}

/* selecting function for tho name list */
void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex) {
	int no = 0, num = 0;
	while(1) {
		printf("\nSelect Name's no (exit:0) >> ");
		scanf("%d", &no);
		if(no == 0) break; // if user input 0, break
		num = nameTokIndex[no-1];
		printf("[NAME%2d] %.*s\n", no, (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the no's name
		printf("%.*s\n", (t+num+1)->end-(t+num+1)->start, jsonstr+(t+num+1)->start); // print the no's value
	}
}

/* put objects from the JSON file into a list */
void objectNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *objectTokIndex) {
	int i, count = 0, parent = 0;
	for(i = 0; i < tokcount; i++) {
		if((t+i)->parent == 0) parent = (t+i)->parent;
		if((t+i)->type == JSMN_OBJECT) { // if the token means object
			if(t->type == JSMN_OBJECT && (t+i+1)->parent == 0 // if the first token is a object
					|| (t->type == JSMN_ARRAY && (t+i)->parent == parent)) { // if the first token is an array
				objectTokIndex[count] = i; // store the token number in the objectTokIndex array
				count++;
			}
		}
		objectTokIndex[count] = tokcount; // store the tokcount for other functions
	}
}

/* print the list of object's names */
void printObjectList(char *jsonstr, jsmntok_t *t, int *objectTokIndex) {
	printf("***** Object List *******\n");

	int count = 0, num = 0;
	while(objectTokIndex[count + 1] != '\0') {
		num = objectTokIndex[count] + 2;
		printf("[NAME%2d] %.*s\n", (count + 1), (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the object's name
		count++;
	}
}

/* print the first name and value of the object */
void printFirstObject(char *jsonstr, jsmntok_t *t, int *objectTokIndex, int no) {
	int name = objectTokIndex[no-1] + 1; // first name of the object
	int value = objectTokIndex[no-1] + 2; // first value of the object
	if((t+objectTokIndex[no])->type != JSMN_ARRAY)
		printf("%.*s : %.*s\n", (t+name)->end - (t+name)->start, jsonstr + (t+name)->start, // print the name
														(t+value)->end - (t+value)->start, jsonstr + (t+value)->start); // print the value
}

/* print the selected object */
void printSelectedObject(char *jsonstr, jsmntok_t *t, int *objectTokIndex, int no) {
	int num = objectTokIndex[no-1]+3, parent = objectTokIndex[no-1]; // where to start printing the selected object
	int end = objectTokIndex[no]; // next object number
	char *STRING;
	STRING = (char *)malloc(100);
	int length = 0;

	while (num < end) {
		if((t+num)->parent == 0) parent = (t+num)->parent;
		length = (t+num)->end-(t+num)->start; // size of the token
		if((t+num)->type == JSMN_STRING && (t+num)->size == 1) {
			if(t->type == JSMN_OBJECT && (t+num)->parent == 0 // if the first token is a object
					|| (t->type == JSMN_ARRAY && (t+num)->parent == parent)) { // if the first token is an array
				length += (t+num+1)->end-(t+num+1)->start; // size of the value
				STRING = realloc(STRING, length + 6); // reallocate for changed length of the string
				strcpy(STRING, "[");
				strncat(STRING, jsonstr+(t+num)->start, (t+num)->end-(t+num)->start);
				strcat(STRING, "]   ");
				strncat(STRING, jsonstr+(t+num+1)->start, (t+num+1)->end-(t+num+1)->start);
				printf("\t%s\n", STRING); // print the STRING ("\t[name]   value")
			}
		}
		num++;
	}
}

/* selecting function for the object's name list */
void selectObjectList(char *jsonstr, jsmntok_t *t, int *objectTokIndex) {
	int no = 0;

	while(1) {
		printf("\n원하는 번호 입력 (Exit:0) :");
		scanf("%d", &no);
		if(no == 0) break; // if user input 0, break
		printFirstObject(jsonstr, t, objectTokIndex, no); // print the first name and value of the object
		printSelectedObject(jsonstr, t, objectTokIndex, no); // print the selected object
	}
}

static char *JSON_STRING;

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
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
	int objectTokIndex[100]; // for index of tokens that point object
	for (i = 0; i < 100; i++){
		nameTokIndex[i] = '\0';
		objectTokIndex[i] = '\0';
	}

	JSON_STRING = readJSONFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	for (i = 0; i < r; i++) {
			printf("t[%d]:%.*s\nparent:%d\n", i, t[i].end-t[i].start,
					JSON_STRING + t[i].start, t[i].parent);
	}
	/* Assume the top-level element is an object */
//	if (r < 1 || t[0].type != JSMN_OBJECT) {
//		printf("Object expected\n");
//		return 1;
//	}

	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printNameList(JSON_STRING, t, nameTokIndex);
	selectNameList(JSON_STRING, t, nameTokIndex);

	objectNameList(JSON_STRING, t, r, objectTokIndex);
	printObjectList(JSON_STRING, t, objectTokIndex);
	selectObjectList(JSON_STRING, t, objectTokIndex);

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
