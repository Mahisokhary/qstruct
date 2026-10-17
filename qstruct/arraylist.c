#include <qstruct/qstruct.internal.h>

struct qstruct_arraylist {
	size_t value_size;	// Size of each value
	size_t capacity;	// How much value fits in current array
	size_t length;		// How much is current array filled
	void *array;		// Current array
};

/*
 * Makes sure there is enough space for one more value
 * Extends the size of array if needed
 */
inline static void _ensure_capacity(qstruct_arraylist_t al) {
	if (al->capacity <= al->length) {
		al->capacity *= 2;	// Doubles size of array
		al->array = realloc(al->array, al->capacity * al->value_size);
	}
}

qstruct_result_t qstruct_arraylist_create(qstruct_arraylist_t *arraylist, size_t value_size, size_t initialize_capacity) {
	if (initialize_capacity == 0) initialize_capacity = QSTRUCT_ARRAYLIST_DEFAULT_INITIALIZE_CAPACITY;
	qstruct_arraylist_t al = malloc(sizeof(struct qstruct_arraylist));
	al->value_size = value_size;
	al->capacity = initialize_capacity;
	al->array = malloc(initialize_capacity * value_size);
	al->length = 0;
	*arraylist = al;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_arraylist_add(qstruct_arraylist_t al, const void *value) {
	_ensure_capacity(al);
	void *dest = al->array + al->length++ * al ->value_size;
	memcpy(dest, value, al->value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_arraylist_get(qstruct_arraylist_t al, void *value, size_t index) {
	if (index >= al->length) return QSTRUCT_RESULT_INDEX_OUTOF_BOUND;
	memcpy(value, al->array + index * al->value_size, al->value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_arraylist_remove(qstruct_arraylist_t al, size_t index) {
	if (index >= al->length) return QSTRUCT_RESULT_INDEX_OUTOF_BOUND;
	void *itemptr = al->array + index * al->value_size;
	memmove(itemptr, itemptr + al->value_size, (al->length - index - 1) * al->value_size);
	al->length--;
	return QSTRUCT_RESULT_OK;
}

size_t qstruct_arraylist_length(qstruct_arraylist_t al) {
	return al->length;
}

qstruct_arraylist_t qstruct_arraylist_clear(qstruct_arraylist_t al) {
	al->length = 0;
	return QSTRUCT_RESULT_OK;
}

qstruct_arraylist_t qstruct_arraylist_destroy(qstruct_arraylist_t al) {
	free(al->array);
	free(al);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_arraylist_set(qstruct_arraylist_t al, const void *value, size_t index) {
	if (index >= al->length) return QSTRUCT_RESULT_INDEX_OUTOF_BOUND;
	memcpy(al->array + index * al->value_size, value, al->value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_arraylist_t qstruct_arraylist_insert(qstruct_arraylist_t al, const void *value, size_t index) {
	_ensure_capacity(al);
	memmove(al->array + (index + 1) * al->value_size, al->array + index * al->value_size, (al->length - index) * al->value_size);
	memcpy(al->array + index * al->value_size, value, al->value_size);
	al->length++;
	return QSTRUCT_RESULT_OK;
}

void* qstruct_arraylist_array(qstruct_arraylist_t al) {
	return al->array;
}

void* qstruct_arraylist_convert_array(qstruct_arraylist_t al, size_t *size) {
	void *arr = al->array;
	if (al->length != al->capacity) {
		arr = realloc(arr, al->length);
	}
	*size = al->length;
	free(al);
	return arr;
}

