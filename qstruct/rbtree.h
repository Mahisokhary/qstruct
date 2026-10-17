#ifndef _qstruct_rbtree_h_
#define _qstruct_rbtree_h_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pointer to actual red-black tree
 * In this implementation we assume given value is a struct
 * and the comparator will compare a specific field in the struct
 * but non-struct types like integers can also be used as value
 */
typedef struct qstruct_rbtree* qstruct_rbtree_t;

/*
 * An iterator for the tree
 */
typedef struct qstruct_rbtree_iterator* qstruct_rbtree_iterator_t;

/*
 * Compares to values and returns a number as result:
 * -1: x is smaller than y
 * 0: values are equal
 * 1: x is bigger than y
 */
typedef int8_t (*qstruct_rbtree_comparator_t)(const char *x, const char *y);

/*
 * Creates a red-black tree
 */
qstruct_result_t qstruct_rbtree_create(qstruct_rbtree_t *tree, qstruct_rbtree_comparator_t comparator);

/*
 * Adds given value to the tree
 * Time complexity: O(log n)
 */
qstruct_result_t qstruct_rbtree_add(qstruct_rbtree_t tree, const void *value, size_t value_size);

/*
 * Gets value in of node in tree
 * if value is a null pointer only value_size is set
 * function will use value_size as size of value pointer given
 * if value_size is zero function will set it automatically
 * value must contains fields needed by comparator
 */
qstruct_result_t qstruct_rbtree_get(qstruct_rbtree_t tree, void *value, size_t *value_size);

/*
 * Gets pointer to value in tree
 * value must contains pointer to a value that contains least value for comparator to find actual value
 * value_size is ignored if its NULL
 * NOTICE: Please dont change anything that will effect comparator result
 * WARNING: changes in tree may break the pointer (removing the value)
 * Time complexity: O(log n)
 */
qstruct_result_t qstruct_rbtree_getp(qstruct_rbtree_t tree, void **value, size_t *value_size);

/*
 * Checks if the value exists in tree
 * Time complexity: O(log n)
 */
bool qstruct_rbtree_has(qstruct_rbtree_t tree, const void *value);

/*
 * Removes given value from tree
 * Time complexity: O(log n)
 */
qstruct_result_t qstruct_rbtree_remove(qstruct_rbtree_t tree, const void *value);

/*
 * Destroys the tree
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_rbtree_destroy(qstruct_rbtree_t tree);

/*
 * Returns length of the tree
 * Time complexity: O(n)
 */
size_t qstruct_rbtree_length(qstruct_rbtree_t tree);

/*
 * Return all values as an array
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_rbtree_iterator_create(qstruct_rbtree_t tree, qstruct_rbtree_iterator_t *iterator);

/*
 * NOTICE: you have to run next on first time
 * If has next goes to next item and return true
 * else return false
 * Time complexity: O(1)
 */
bool qstruct_rbtree_iterator_next(qstruct_rbtree_iterator_t iterator);

/*
 * Return size of the current value
 * Time complexity: O(1)
 */
size_t qstruct_rbtree_iterator_current_size(qstruct_rbtree_iterator_t iterator);

/*
 * Get value of current value and store it in given buffer
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_rbtree_iterator_current_value(qstruct_rbtree_iterator_t iterator, void *buffer);

/*
 * Get pointer to value of current value
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_rbtree_iterator_current_valuep(qstruct_rbtree_iterator_t iterator, void **buffer);

/*
 * Destroys the iterator
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_rbtree_iterator_destroy(qstruct_rbtree_iterator_t iterator);

#ifdef __cplusplus
}
#endif
#endif
