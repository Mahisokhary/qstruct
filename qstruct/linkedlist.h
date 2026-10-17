#ifndef _qstruct_linkedlist_h_
#define _qstruct_linkedlist_h_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * A pointer to actual linkedlist
 * This implemetation is a doubly linked list
 */
typedef struct qstruct_linkedlist* qstruct_linkedlist_t;

/*
 * Creates an empty linkedlist
 */
qstruct_result_t qstruct_linkedlist_create(qstruct_linkedlist_t *list);

/*
 * Adds value to end of the list
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_linkedlist_add(qstruct_linkedlist_t list, const void *value, size_t value_size);

/*
 * Gets pointer of value at given index
 * Only use it when you’re sure the value won’t be removed or changed by another function or thread
 * Time complexity: O(n) (in worst case n/2)
 */
qstruct_result_t qstruct_linkedlist_getp(qstruct_linkedlist_t list, size_t index, void **value, size_t *value_size);

/*
 * Gets value at given index
 * Time complexity: O(n) (in worst case n/2)
 */
qstruct_result_t qstruct_linkedlist_get(qstruct_linkedlist_t list, size_t index, void *value, size_t *value_size);

/*
 * Destroy the list and free used memory
 */
qstruct_result_t qstruct_linkedlist_destroy(qstruct_linkedlist_t list);

/*
 * Returns length of the list
 * Time complexity: O(1)
 */
size_t qstruct_linkedlist_length(qstruct_linkedlist_t list);

/*
 * Removes item at given index
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_linkedlist_remove(qstruct_linkedlist_t list, size_t index);

/*
 * Clears the list
 * Time complexity O(n)
 */
qstruct_result_t qstruct_linkedlist_clear(qstruct_linkedlist_t list);

/*
 * Replace value at given index with given value
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_linkedlist_replace(qstruct_linkedlist_t list, size_t index, const void *value, size_t value_size);

/*
 * Insert value at given index
 */
qstruct_result_t qstruct_linkedlist_insert(qstruct_linkedlist_t list, size_t index, const void *value, size_t value_size);

#ifdef __cplusplus
}
#endif
#endif
