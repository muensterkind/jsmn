#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include "productlist.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

/* read the JSON file */
char *readJSONFile() {
	char fileName[20] = "", address[50] = "/home/u21700648/project/jsmn/";
	printf("원하는 파일명 입력 :"); // choose the file when the program is running
	scanf("%s", fileName);
	strcat(fileName, ".json");
	strcat(address, fileName);
	FILE *fp = NULL;
	fp = fopen(address, "r"); // open the chosen file
	if(fp == NULL) {
		printf("%s 파일이 존재하지 않습니다.\n", fileName); // if the file doesn't exist
		return "-1";
	}
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

	//puts(STRING);
	fclose(fp);
	return STRING;
}

/* put names from the JSON file into a list */
void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, NameTokenInfo *nameTokenInfo) {
	int i, count = 0, parent = 0;
	for(i = 0; i < tokcount; i++) {
		//printf("token%2d size: %d type: %d\n parent: %d", i, (t+i)->size, (t+i)->type, (t+i)->parent);
		if((t+i)->type == JSMN_STRING && (t+i)->size == 1) { // if the token means name
			if((t->type == JSMN_OBJECT && (t+1)->type == JSMN_STRING) && (t+2)->type == JSMN_ARRAY) { // if the token is in the first object's array
				if((t+i)->parent == 0) continue; // if the token is array's name skip it
				nameTokenInfo[count].tokindex = i; // store the token number in the Index->tokindex array
				count++;
			}
		}
	}
}

/* print the list of names */
void printNameList(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo) {
	printf("***** Name List *******\n");

	int count = 0, num = 0;
	while(nameTokenInfo[count].tokindex != '\0') {
		num = nameTokenInfo[count].tokindex;
		printf("[NAME%2d] %.*s\n", (count + 1), (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the name
		count++;
	}
}

/* selecting function for tho name list */
void selectNameList(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo) {
	int no = 0, num = 0;
	while(1) {
		printf("\nSelect Name's no (exit:0) >> ");
		scanf("%d", &no);
		if(no == 0) break; // if user input 0, break
		num = nameTokenInfo[no-1].tokindex;
		printf("[NAME%2d] %.*s\n", no, (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the no's name
		printf("%.*s\n", (t+num+1)->end-(t+num+1)->start, jsonstr+(t+num+1)->start); // print the no's value
	}
}

/* put objects from the JSON file into a list */
void objectNameList(char *jsonstr, jsmntok_t *t, int tokcount, NameTokenInfo *nameTokenInfo) {
	int i, count = 0, parent = 0;
	for(i = 1; i < tokcount; i++) {
		if((t+i)->type == JSMN_OBJECT) { // if the token means object
			if((t->type == JSMN_OBJECT && (t+1)->type == JSMN_STRING) && (t+2)->type == JSMN_ARRAY) { // if the token is in the first object's array
				nameTokenInfo[count].objectindex = i; // store the token number in the Index->objectindex array
				count++;
			}
		}
		nameTokenInfo[count].objectindex = tokcount; // store the tokcount for other functions
	}
}

/* print the list of object's names */
void printObjectList(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo) {
	printf("***** Object List *******\n");

	int count = 0, num = 0;
	while(nameTokenInfo[count + 1].objectindex != '\0') {
		num = nameTokenInfo[count].objectindex + 2;
		printf("[NAME%2d] %.*s\n", (count + 1), (t+num)->end-(t+num)->start, jsonstr+(t+num)->start); // print the object's name
		count++;
	}
}

/* print the first name and value of the object */
void printFirstObject(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo, int no) {
	int name = nameTokenInfo[no - 1].objectindex + 1; // first name of the object
	int value = nameTokenInfo[no - 1].objectindex + 2; // first value of the object
	if((t + nameTokenInfo[no].objectindex)->type != JSMN_ARRAY)
		printf("%.*s : %.*s\n", (t+name)->end - (t+name)->start, jsonstr + (t+name)->start, // print the name
														(t+value)->end - (t+value)->start, jsonstr + (t+value)->start); // print the value
}

/* print the selected object */
void printSelectedObject(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo, int no) {
	int num = nameTokenInfo[no - 1].objectindex + 3, parent = nameTokenInfo[no - 1].objectindex; // where to start printing the selected object
	int end = nameTokenInfo[no].objectindex; // last object number
	char *STRING;
	STRING = (char *)malloc(100);
	int length = 0;

	while (num < end) {
		if((t+num)->parent == 0) parent = (t+num)->parent;
		length = (t+num)->end-(t+num)->start; // size of the token
		if((t+num)->type == JSMN_STRING && (t+num)->size == 1) {
			if((t->type == JSMN_OBJECT && (t+1)->type == JSMN_STRING) && (t+2)->type == JSMN_ARRAY) { // if the token is in the first object's array
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
void selectObjectList(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo) {
	int no = 0;

	while(1) {
		printf("\n원하는 번호 입력 (Exit:0) :");
		scanf("%d", &no);
		if(no == 0) break; // if user input 0, break
		printFirstObject(jsonstr, t, nameTokenInfo, no); // print the first name and value of the object
		printSelectedObject(jsonstr, t, nameTokenInfo, no); // print the selected object
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
	NameTokenInfo Index[100]; // for index of tokens that point names and objects
	for (i = 0; i < 100; i++){
		Index[i].tokindex = '\0';
		Index[i].objectindex = '\0';
	}

	JSON_STRING = readJSONFile();
	if(JSON_STRING == "-1") return EXIT_SUCCESS;

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

	jsonNameList(JSON_STRING, t, r, Index);
	printNameList(JSON_STRING, t, Index);
	selectNameList(JSON_STRING, t, Index);

	objectNameList(JSON_STRING, t, r, Index);
	printObjectList(JSON_STRING, t, Index);
	selectObjectList(JSON_STRING, t, Index);

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
