#ifndef _qstruct_qstruct_h_
#define _qstruct_qstruct_h_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Result of qstruct functions
 */
typedef enum {
	QSTRUCT_RESULT_OK = 0,		// successful
	QSTRUCT_RESULT_INDEX_OUTOF_BOUND = 1, // no value is stored in given index
	QSTRUCT_RESULT_VALUE_NOT_FOUND = 2,	// requested value not found
	QSTRUCT_RESULT_KEY_NOT_FOUND = 3,	// requested key not found
} qstruct_result_t;

/*
 * Returns qstruct result enum constants as string
 */
const char *qstruct_result_string(qstruct_result_t res);

#ifdef __cplusplus
}
#endif

#include "arraylist.h"
#include "linkedlist.h"
#include "rbtree.h"
#include "hashmap.h"

#endif
