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


#ifdef __cplusplus
}
#endif

#endif /* __PRODUCTLIST_H_ */
