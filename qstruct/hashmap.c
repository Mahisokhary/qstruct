#include <qstruct/qstruct.internal.h>

/*
 * Detects which bucket the key should be in
 */
#define HM_BUCKET_INDEX(k, ks, hm) (hm->hasher(k, ks, hm->seed) & (hm->capacity - 1 ))

struct qstruct_hashmap {
	size_t length;			// How much is current array filled
	qstruct_rbtree_t *buckets;	// Buckets
	size_t capacity;		// Capacity of buckets
	qstruct_rbtree_comparator_t comparator;	// Used for handling
	double max_loadfactor;			// max load factor
	qstruct_hashmap_hasher_t hasher;	// Hash function
	long seed;				// Seed for hash function
};

struct entry {
	qstruct_hashmap_t map;
	char *value;
	size_t value_size;
	size_t key_size;
	uint8_t key[];
};

struct iterator {
	qstruct_hashmap_t map;
	size_t size;
	int index;
	struct entry *entries[];
};

/*
 * Compares two entries
 */
static int8_t _hm_entry_comparator(const char *x, const char *y) {
	const struct entry *e1 = (const void *) x;
	const struct entry *e2 = (const void *) y;
	return e1->map->comparator(e1->key, e2->key);
}

/*
 * Get bucket for given entry
 * Creates the bucket if doesnt exists
 */
static inline qstruct_result_t _hm_get_bucket(qstruct_hashmap_t hm, qstruct_rbtree_t *b, struct entry *e) {
	int bucket_index = HM_BUCKET_INDEX(e->key, e->key_size, hm);
	qstruct_rbtree_t bucket = hm->buckets[bucket_index];
	if (bucket == NULL) {
		qstruct_run(qstruct_rbtree_create(&bucket, &_hm_entry_comparator));
		hm->buckets[bucket_index] = bucket;
	}
	*b = bucket;
	return QSTRUCT_RESULT_OK;
}

/*
 * Add entry to the buckets
 * In this function load factor is not checked
 */
static inline qstruct_result_t _hm_put(qstruct_hashmap_t hm, struct entry *e) {
	qstruct_rbtree_t b;
	qstruct_run(_hm_get_bucket(hm, &b, e));
	qstruct_run(qstruct_rbtree_add(b, e, sizeof(struct entry) + e->key_size));
	return QSTRUCT_RESULT_OK;
}

/*
 * Make sure load factor is below max load factor
 * If load factor is above max load it will doubles the buckets size
 */
static inline qstruct_result_t _hm_ensure_loadfactor(qstruct_hashmap_t hm) {
	if (hm->length / hm->capacity >= hm->max_loadfactor) {
		qstruct_rbtree_t *obuckets = hm->buckets;
		size_t ocapacity = hm->capacity;

		size_t ncapacity = hm->capacity * 2;
		qstruct_rbtree_t *buckets = calloc(ncapacity, sizeof(qstruct_rbtree_t));
		hm->buckets = buckets;
		hm->capacity = ncapacity;

		qstruct_rbtree_iterator_t it;
		for (int i = 0; i < ocapacity; i++) {
			qstruct_rbtree_t ob = obuckets[i];
			if (ob != NULL) {
				qstruct_run(qstruct_rbtree_iterator_create(ob, &it));
				while (qstruct_rbtree_iterator_next(it)) {
					struct entry *e;
					size_t e_size = qstruct_rbtree_iterator_current_size(it);
					qstruct_run(qstruct_rbtree_iterator_current_valuep(it, (void **) &e));
					qstruct_run(_hm_put(hm, e));
				}
				qstruct_run(qstruct_rbtree_destroy(ob));
			}
		}
		free(obuckets);
	}


	return QSTRUCT_RESULT_OK;
}

/*
 * Finds and returns entry of the given key
 */
static inline qstruct_result_t _hm_get(qstruct_hashmap_t hm, struct entry **e, const char *key, size_t key_size) {
	qstruct_rbtree_t bucket = hm->buckets[HM_BUCKET_INDEX(key, key_size, hm)];
	if (bucket == NULL) return QSTRUCT_RESULT_KEY_NOT_FOUND;

	struct entry *tempe = malloc(sizeof(struct entry) + key_size);
	tempe->map = hm;
	tempe->key_size = key_size;
	memcpy(tempe->key, key, key_size);

	*e = tempe;
	size_t esize;

	qstruct_result_t res = qstruct_rbtree_getp(bucket, (void **) e, &esize);
	if (res == QSTRUCT_RESULT_VALUE_NOT_FOUND) return QSTRUCT_RESULT_KEY_NOT_FOUND;
	else if (res != QSTRUCT_RESULT_OK) return res;

	free(tempe);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_create(qstruct_hashmap_t *hashmap, qstruct_rbtree_comparator_t comparator, size_t capacity, double max_loadfactor, qstruct_hashmap_hasher_t hasher, long seed) {
	if (capacity == 0) capacity = QSTRUCT_HASHMAP_DEFAULT_CAPACITY;
	if (max_loadfactor == 0) max_loadfactor = QSTRUCT_HASHMAP_DEFAULT_MAX_LOADFACTOR;

	qstruct_hashmap_t hm = malloc(sizeof(struct qstruct_hashmap));
	hm->length = 0;
	hm->capacity = capacity;
	hm->comparator = comparator;
	hm->max_loadfactor = max_loadfactor;
	hm->hasher = hasher;
	hm->seed = seed;

	qstruct_rbtree_t *buckets = calloc(capacity, sizeof(qstruct_rbtree_t));
	hm->buckets = buckets;

	*hashmap = hm;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_destroy(qstruct_hashmap_t hm) {
	qstruct_rbtree_t *buckets = hm->buckets;

	for (int i = 0; i < hm->capacity; i++) {
		qstruct_rbtree_t bucket = buckets[i];
		if (bucket != NULL) {
			qstruct_rbtree_iterator_t it;
			qstruct_run(qstruct_rbtree_iterator_create(bucket, &it));
			while (qstruct_rbtree_iterator_next(it)) {
				struct entry *e;
				qstruct_run(qstruct_rbtree_iterator_current_valuep(it, (void **) &e));
				free(e->value);
			}
			qstruct_run(qstruct_rbtree_iterator_destroy(it));
			qstruct_run(qstruct_rbtree_destroy(bucket));
		}
	}

	free(buckets);
	free(hm);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_add(qstruct_hashmap_t hm, const void *key, size_t key_size, const void *value, size_t value_size) {
	qstruct_run(_hm_ensure_loadfactor(hm));

	struct entry *e = malloc(sizeof(struct entry) + key_size);
	char *e_val = malloc(value_size);

	e->map = hm;
	e->value = e_val;
	e->value_size = value_size;
	e->key_size = key_size;

	memcpy(e->key, key, key_size);
	if (value) memcpy(e->value, value, value_size);

	qstruct_run(_hm_put(hm, e));
	free(e);
	hm->length++;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_get(qstruct_hashmap_t hm, const void *key, size_t key_size, void *value, size_t *value_size) {
	void *src;
	size_t rvalue_size;
	qstruct_run(qstruct_hashmap_getp(hm, key, key_size, &src, &rvalue_size));

	if (*value_size == 0) *value_size = rvalue_size;
	if (value != NULL) memcpy(value, src, *value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_getp(qstruct_hashmap_t hm, const void *key, size_t key_size, void **value, size_t *value_size) {
	struct entry *e;
	qstruct_run(_hm_get(hm, &e, key, key_size));
	*value = e->value;
	if (value_size) *value_size = e->value_size;
	return QSTRUCT_RESULT_OK;
}

bool qstruct_hashmap_has(qstruct_hashmap_t hm, const void *key, size_t key_size) {
	struct entry *e;
	return _hm_get(hm, &e, key, key_size) == QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_remove(qstruct_hashmap_t hm, const void *key, size_t key_size) {
	struct entry *e;
	qstruct_run(_hm_get(hm, &e, key, key_size));
	long bucket_index = HM_BUCKET_INDEX(key, key_size, hm);
	qstruct_rbtree_t bucket = hm->buckets[bucket_index];
	free(e->value);
	qstruct_run(qstruct_rbtree_remove(bucket, e));
	if (qstruct_rbtree_length(bucket) == 0) {
		qstruct_run(qstruct_rbtree_destroy(bucket));
		hm->buckets[bucket_index] = NULL;
	}
	hm->length--;
	return QSTRUCT_RESULT_OK;
}

size_t qstruct_hashmap_length(qstruct_hashmap_t hm) {
	return hm->length;
}

qstruct_result_t qstruct_hashmap_iterator_create(qstruct_hashmap_t hm, qstruct_hashmap_iterator_t *iterator) {
	struct iterator *it = malloc(sizeof(struct iterator) + sizeof(struct entry*) * hm->length);
	it->map = hm;
	it->size = 0;
	it->index = -1;

	for (int i = 0; i < hm->capacity; i++) {
		qstruct_rbtree_t b = hm->buckets[i];
		if (b != NULL) {
			qstruct_rbtree_iterator_t it2;
			qstruct_run(qstruct_rbtree_iterator_create(b, &it2));
			while (qstruct_rbtree_iterator_next(it2)) {
				struct entry **e = &it->entries[it->size++];
				qstruct_run(qstruct_rbtree_iterator_current_valuep(it2, (void**) e));
			}
			qstruct_run(qstruct_rbtree_iterator_destroy(it2));
		}
	}

	*iterator = it;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_iterator_destroy(qstruct_hashmap_iterator_t iterator) {
	free(iterator);
	return QSTRUCT_RESULT_OK;
}

bool qstruct_hashmap_iterator_next(qstruct_hashmap_iterator_t iterator) {
	struct iterator *it = iterator;
	if (it->index + 1 >= it->size) return false;
	it->index++;
	return true;
}

size_t qstruct_hashmap_iterator_current_key_size(qstruct_hashmap_iterator_t iterator) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	return e->key_size;
}

qstruct_result_t qstruct_hashmap_iterator_current_key_get(qstruct_hashmap_iterator_t iterator, void *key) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	memcpy(key, e->key, e->key_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_iterator_current_key_getp(qstruct_hashmap_iterator_t iterator, void **key) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	*key = e->key;
	return QSTRUCT_RESULT_OK;
}

size_t qstruct_hashmap_iterator_current_value_size(qstruct_hashmap_iterator_t iterator) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	return e->value_size;
}

qstruct_result_t qstruct_hashmap_iterator_current_value_get(qstruct_hashmap_iterator_t iterator, void *value) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	memcpy(value, e->value, e->value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_hashmap_iterator_current_value_getp(qstruct_hashmap_iterator_t iterator, void **value) {
	struct iterator *it = iterator;
	struct entry *e = it->entries[it->index];
	*value = e->value;
	return QSTRUCT_RESULT_OK;
}

