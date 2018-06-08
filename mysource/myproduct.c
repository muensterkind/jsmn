#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../jsmn.h"
#include "myproduct.h"


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

/* store JSON_STRING's tokens in product structures */
int makeProduct(const char *json, jsmntok_t *t, int tokcount, product_t * p[]) {
	int i, count = 0;

	for(i = 1; i < tokcount; i++) {
		if((t+i)->type == JSMN_OBJECT && (t+i)->parent->type == JSMN_ARRAY) { // if the token means product object
			p[count] = (product_t *)malloc(sizeof(product_t));
			assert(p[count] != NULL);
			inputInfo(json, t, i + 1, p[count]);
			count++;
		}
	}
	return count;
}

/* input information into the productList */
void inputInfo(char *json, jsmntok_t *t, int tokNum, product_t *product) {
	while((t+tokNum)->type != JSMN_OBJECT) {
		if (jsoneq(json, &t[tokNum], "company") == 0) product->company = checkCompany(tokNum+1);
		else if (jsoneq(json, &t[tokNum], "name") == 0)
			strncpy(product->name, json + (t+tokNum+1)->start, (t+tokNum+1)->end - (t+tokNum+1)->start);
		else if (jsoneq(json, &t[tokNum], "price") == 0) product->price = stringToInt(json, t, tokNum+1);
		else if (jsoneq(json, &t[tokNum], "count") == 0) product->count = stringToInt(json, t, tokNum+1);
		tokNum += 2;
	}
}

/* check the company name */
company_t checkCompany(int num) {
	company_t value = 0;
	if (jsoneq(JSON_STRING, &t[num], "탐사") == 0) value = TAMSAA;
	else if (jsoneq(JSON_STRING, &t[num], "농심") == 0) value = NONGSHIM;
	else if (jsoneq(JSON_STRING, &t[num], "다우니") == 0) value = DOWNY;
	else if (jsoneq(JSON_STRING, &t[num], "센스") == 0) value = SENSE;
	else value = -1;
	return value;
}

/* print objects list */
void printList(char *jsonstr, jsmntok_t *t, NameTokenInfo *nameTokenInfo) {
	printf("************************************************\n");
	printf("번호    제품명  제조사  가격    개수    총가격  \n");
	int objectNo = 0;
	int company = 0, name = 0, price = 0, count = 0;
	int priceValue = 0, countValue = 0;

	while(nameTokenInfo[objectNo + 1].objectindex != '\0') {
		name = giveTokindex(jsonstr, t, nameTokenInfo, objectNo, "name"); // invoke name's tokindex
		company = giveTokindex(jsonstr, t, nameTokenInfo, objectNo, "company"); // invoke company's tokindex
		price = giveTokindex(jsonstr, t, nameTokenInfo, objectNo, "price"); // invoke price's tokindex
		priceValue = stringToInt(jsonstr, t, price); // change the string into int value
		count = giveTokindex(jsonstr, t, nameTokenInfo, objectNo, "count"); // invoke count's tokindex
		countValue = stringToInt(jsonstr, t, count); // change the string into int value

		printf("%-8d", objectNo + 1);
		printf("%-11.*s", (t+name)->end - (t+name)->start, jsonstr + (t+name)->start);
		printf("%-10.*s", (t+company)->end - (t+company)->start, jsonstr + (t+company)->start);
		printf("%-8d", priceValue);
		printf("%-8d", countValue);
		printf("%-8d\n", priceValue * countValue);

		objectNo++;
	}
	printf("************************************************\n");
}

/* change token string that means integer into int type */
int stringToInt(char *jsonstr, jsmntok_t *t, int num) {
	char *STRING = "";
	STRING = (char *)malloc((t+num)->end - (t+num)->start);
	strncpy(STRING, jsonstr + (t+num)->start, (t+num)->end - (t+num)->start);
	int value = atoi(STRING);
	free(STRING);
	return value;
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
	int i = 0;
	int r = 0;
	int pcount = 0;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	product_t *coupangList[20]; // for saving product information

	JSON_STRING = readJSONFile();
	if(JSON_STRING == "-1") return EXIT_SUCCESS;

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	pcount = makeProduct(JSON_STRING, t, r, coupangList);

	printList(JSON_STRING, t, Index);

	return EXIT_SUCCESS;
}
