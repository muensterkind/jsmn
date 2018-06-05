#ifndef __PRODUCTLIST_H_
#define __PRODUCTLIST_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Contains tokindex and objectindex.
 */
typedef struct {
	int tokindex; /* store each token's index */
	int objectindex; /* store each object's index */
} NameTokenInfo;

/**
 * Stores company names in enum type.
 */
typedef enum {
	TAMSAA = 0,    // 탐사
	NONGSHIM = 1,  // 농심
	DOWNY = 2,     // 다우
	SENSE = 3,     // 센스
} company_t;

/**
 * Contains information(company, name, price, count) of product.
 */
typedef struct {
	company_t company; // 제조사
	char name[20];		 // 제품명
	int price;				 // 가격
	int count;				 //	개수
} product_t;

#ifdef __cplusplus
}
#endif

#endif /* __PRODUCTLIST_H_ */
