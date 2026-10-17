#ifndef _qstruct_arraylist_h_
#define _qstruct_arraylist_h_
#ifdef __cplusplus
extern "C" {
#endif

#define QSTRUCT_ARRAYLIST_DEFAULT_INITIALIZE_CAPACITY	16

/*
 * A pointer to actual arraylist
 */
typedef struct qstruct_arraylist* qstruct_arraylist_t;

/*
 * Creates an arraylist
 * value_size: size of each value in array
 * initialize_capacity: initial size of array (if 0 value of QSTRUCT_ARRAYLIST_DEFAULT_INITIALIZE_CAPACITY is used instead)
 */
qstruct_result_t qstruct_arraylist_create(qstruct_arraylist_t *arraylist, size_t value_size, size_t initialize_capacity);

/*
 * Adds a value to arraylist
 * arraylist: the array you wanna add value to
 * value: the value
 * Time complexity: if enought capacity O(1) else O(n)
 */
qstruct_result_t qstruct_arraylist_add(qstruct_arraylist_t arraylist, const void *value);

/*
 * Get a value from arraylist
 * index: index of value
 * value: value is copied to given pointer
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_arraylist_get(qstruct_arraylist_t arraylist, void *value, size_t index);

/*
 * Removes a value from arraylist
 * index: index of the value
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_arraylist_remove(qstruct_arraylist_t arraylist, size_t index);

/*
 * Returns length of the arraylist
 */
size_t qstruct_arraylist_length(qstruct_arraylist_t arraylist);

/*
 * Replace value at given index with given value
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_arraylist_set(qstruct_arraylist_t arraylist, const void *value, size_t index);

/*
 * Insert given value at given index
 * Time complexity: O(n)
 */
qstruct_arraylist_t qstruct_arraylist_insert(qstruct_arraylist_t arraylist, const void *value, size_t index);

/*
 * Clear the arraylist
 * Time complexity: O(1)
 */
qstruct_arraylist_t qstruct_arraylist_clear(qstruct_arraylist_t arraylist);

/*
 * Destroys arraylist and free used memory
 */
qstruct_arraylist_t qstruct_arraylist_destroy(qstruct_arraylist_t arraylist);

/*
 * Returns the array
 */
void* qstruct_arraylist_array(qstruct_arraylist_t al);

/*
 * Destroys the qstruct arraylist metadata
 * Resize the array of arraylist to length of it
 * Returns the array
 */
void* qstruct_arraylist_convert_array(qstruct_arraylist_t al, size_t *size);

#ifdef __cplusplus
}
#endif
#endif
