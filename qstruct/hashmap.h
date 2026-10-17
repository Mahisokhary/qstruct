#ifndef _qstruct_hashmap_h_
#define _qstruct_hashmap_h_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTICE: The bad day condition mentioned in time complexity informations
 *         is directly effected by hash collisions
 *         and can be effected by max loadfactor, hasher and initial capacity
 */

#define QSTRUCT_HASHMAP_DEFAULT_CAPACITY	16
#define QSTRUCT_HASHMAP_DEFAULT_MAX_LOADFACTOR	0.5

/*
 * A pointer to actual hashmap
 */
typedef struct qstruct_hashmap* qstruct_hashmap_t;

/*
 * An iterator for the map
 */
typedef void* qstruct_hashmap_iterator_t;

/*
 * Hash the given buffer
 */
typedef long (*qstruct_hashmap_hasher_t)(const char *x, size_t size, long seed);

/*
 * Creates a hashmap
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_create(qstruct_hashmap_t *hashmap, qstruct_rbtree_comparator_t comparator, size_t capacity, double max_loadfactor, qstruct_hashmap_hasher_t hasher, long seed);

/*
 * Destroy the hashmap
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_hashmap_destroy(qstruct_hashmap_t hashmap);

/*
 * Add key and value to the tree
 * Time complexity: O(1) or O(log n) in a bad day
 * value: if null value will remain un initialized
 */
qstruct_result_t qstruct_hashmap_add(qstruct_hashmap_t hashmap, const void *key, size_t key_size, const void *value, size_t value_size);

/*
 * Get value by key
 * if value is a null pointer only value_size is set
 * function will use value_size as size of value pointer given
 * if value_size is zero function will set it automatically
 * Time complexity: O(1) or O(log n) in a bad day
 */
qstruct_result_t qstruct_hashmap_get(qstruct_hashmap_t hashmap, const void *key, size_t key_size, void *value, size_t *value_size);

/*
 * Gets pointer to the value of the key
 * value_size is ignored if its NULL
 * Time complexity: O(1) or O(log n) in a bad day
 */
qstruct_result_t qstruct_hashmap_getp(qstruct_hashmap_t hashmap, const void *key, size_t key_size, void **value, size_t *value_size);

/*
 * Checks if key exists in the map
 * Time complexity: O(1) or O(log n) in a bad day
 */
bool qstruct_hashmap_has(qstruct_hashmap_t hashmap, const void *key, size_t key_size);

/*
 * Removes given key from map
 * Time complexity: O(1) or O(log n) in a bad day
 */
qstruct_result_t qstruct_hashmap_remove(qstruct_hashmap_t hashmap, const void *key, size_t key_size);

/*
 * Returns length of the map
 * Time complexity: O(1)
 */
size_t qstruct_hashmap_length(qstruct_hashmap_t hashmap);

/*
 * Creates an iterator for the map
 * Time complexity: O(n)
 */
qstruct_result_t qstruct_hashmap_iterator_create(qstruct_hashmap_t tree, qstruct_hashmap_iterator_t *iterator);

/*
 * Destroys the iterator
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_iterator_destroy(qstruct_hashmap_iterator_t iterator);

/*
 * NOTICE: you have to run next on first time
 * If has next goes to next item and return true
 * else return false
 * Time complexity: O(1)
 */
bool qstruct_hashmap_iterator_next(qstruct_hashmap_iterator_t iterator);

/*
 * Returns size of current entry key
 * Time complexity: O(1)
 */
size_t qstruct_hashmap_iterator_current_key_size(qstruct_hashmap_iterator_t iterator);

/*
 * Gets key of current entry
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_iterator_current_key_get(qstruct_hashmap_iterator_t iterator, void *key);

/*
 * NOTICE: Changing key value will break the map
 * Gets key of current entry as pointer
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_iterator_current_key_getp(qstruct_hashmap_iterator_t iterator, void **key);

/*
 * Returns size of current entry value
 * Time complexity: O(1)
 */
size_t qstruct_hashmap_iterator_current_value_size(qstruct_hashmap_iterator_t iterator);

/*
 * Gets value of current entry
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_iterator_current_value_get(qstruct_hashmap_iterator_t iterator, void *value);

/*
 * Gets value of current entry as pointer
 * Time complexity: O(1)
 */
qstruct_result_t qstruct_hashmap_iterator_current_value_getp(qstruct_hashmap_iterator_t iterator, void **value);

#ifdef __cplusplus
}
#endif
#endif
