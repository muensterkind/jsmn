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

/* check the company name */
company_t checkCompany(char *json, jsmntok_t *t, int num) {
	company_t value = 0;
	if (jsoneq(json, &t[num], "탐사") == 0) value = TAMSAA;
	else if (jsoneq(json, &t[num], "농심") == 0) value = NONGSHIM;
	else if (jsoneq(json, &t[num], "다우니") == 0) value = DOWNY;
	else if (jsoneq(json, &t[num], "센스") == 0) value = SENSE;
	else value = -1;
	return value;
}

/* input information into the productList */
void inputInfo(char *json, jsmntok_t *t, int tokNum, product_t *product) {
	while((t+tokNum)->type != JSMN_OBJECT) {
		if (jsoneq(json, &t[tokNum], "company") == 0) product->company = checkCompany(json, t, tokNum+1);
		else if (jsoneq(json, &t[tokNum], "name") == 0)
			strncpy(product->name, json + (t+tokNum+1)->start, (t+tokNum+1)->end - (t+tokNum+1)->start);
		else if (jsoneq(json, &t[tokNum], "price") == 0) product->price = stringToInt(json, t, tokNum+1);
		else if (jsoneq(json, &t[tokNum], "count") == 0) product->count = stringToInt(json, t, tokNum+1);
		tokNum += 2;
	}
}

/* store JSON_STRING's tokens in product structures */
int makeProduct(char *json, jsmntok_t *t, int tokcount, product_t * p[]) {
	int i, count = 0;

	for(i = 1; i < tokcount; i++) {
		if((t+i)->type == JSMN_OBJECT && (t+(t+i)->parent)->type == JSMN_ARRAY) { // if the token means product object
			p[count] = (product_t *)malloc(sizeof(product_t));
			assert(p[count] != NULL);
			inputInfo(json, t, i + 1, p[count]);
			count++;
		}
	}
	return count;
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

void printProduct(product_t * p[], int pcount) {
	int i = 0;
	printf("****************************************\n");
	printf("번호    제품명  제조사  가격    개수    \n");
	for(i = 0; i < pcount; i++) {
		printf("%-8d", i + 1);
		printf("%-10s", p[i]->name);
		printf("%-10c", p[i]->company);
		printf("%-8d", p[i]->price);
		printf("%-8d\n", p[i]->count);
	}
	printf("****************************************\n");
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
	int r = 0;
	int pcount = 0; // for counting the number of products
	jsmn_parser p; // parser
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	product_t *coupangList[20]; // for saving product information

	JSON_STRING = readJSONFile(); // read input file as a string
	if(JSON_STRING == "-1") return EXIT_SUCCESS; // if failing to read the file, exit

	jsmn_init(&p); // initialize the parser
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0])); // parse the string into tokens and store the number of tokens
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	pcount = makeProduct(JSON_STRING, t, r, coupangList); // store the product information and save the number of products
	printProduct(coupangList, pcount); // print products

	return EXIT_SUCCESS;
}
