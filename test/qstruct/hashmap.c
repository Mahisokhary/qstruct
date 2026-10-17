#include <test/test.h>
#include <qstruct/qstruct.h>
#include <xxhash.h>
#include <string.h>

static int8_t _comparator(const char *x, const char *y) {
	return strcmp(x, y);
}

static long _hasher(const char *x, size_t size, long seed) {
	return XXH64(x, size, seed);
}


bool test_qstruct_hashmap_add_get() {
	test_run_log("qstruct_hashmap_add_get");
	qstruct_hashmap_t map;
	char res[2];
	size_t res_size = array_len(res);

	bool success = 1;
	success &= qstruct_hashmap_create(&map, &_comparator, 0, 0, &_hasher, 0) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_add(map, "sik", 4, "k", 2) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_add(map, "kir", 4, "s", 2) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_get(map, "sik", 4, res, &res_size) == QSTRUCT_RESULT_OK;
	success &= strcmp(res, "k") == 0;
	success &= qstruct_hashmap_get(map, "kir", 4, res, &res_size) == QSTRUCT_RESULT_OK;
	success &= strcmp(res, "s") == 0;
	success &= qstruct_hashmap_length(map) == 2;
	success &= qstruct_hashmap_destroy(map) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_hashmap_has() {
	test_run_log("qstruct_hashmap_has");
	qstruct_hashmap_t map;

	bool success = 1;
	success &= qstruct_hashmap_create(&map, &_comparator, 0, 0, &_hasher, 0) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_add(map, "sik", 4, "k", 2) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_has(map, "sik", 4);
	success &= !qstruct_hashmap_has(map, "kir", 4);
	success &= qstruct_hashmap_destroy(map) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_hashmap_remove() {
	test_run_log("qstruct_hashmap_remove");
	qstruct_hashmap_t map;
	char res[2];
	size_t res_size = array_len(res);

	bool success = 1;
	success &= qstruct_hashmap_create(&map, &_comparator, 0, 0, &_hasher, 0) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_add(map, "sik", 4, "k", 2) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_has(map, "sik", 4);
	success &= qstruct_hashmap_remove(map, "sik", 4) == QSTRUCT_RESULT_OK;
	success &= !qstruct_hashmap_has(map, "sik", 4);
	success &= qstruct_hashmap_length(map) == 0;
	success &= qstruct_hashmap_destroy(map) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_hashmap_iterator() {
	test_run_log("qstruct_hashmap_iterator");
	qstruct_hashmap_t map;
	qstruct_hashmap_iterator_t it;
	char values[4][5] = { "sik1", "sik2", "sik3", "sik4" };
	char key[5];
	char value[2];

	bool success = 1;
	success &= qstruct_hashmap_create(&map, &_comparator, 0, 0, &_hasher, 0) == QSTRUCT_RESULT_OK;
	for (int i = 0; i < array_len(values); i++) {
		success &= qstruct_hashmap_add(map, values[i], 5, "k", 2) == QSTRUCT_RESULT_OK;
	}
	success &= qstruct_hashmap_iterator_create(map, &it) == QSTRUCT_RESULT_OK;
	while (qstruct_hashmap_iterator_next(it)) {
		success &= qstruct_hashmap_iterator_current_key_size(it) == 5;
		success &= qstruct_hashmap_iterator_current_key_get(it, key) == QSTRUCT_RESULT_OK;
		bool found = false;
		for (int i = 0; i < array_len(values) && !found; i++)
			if (strcmp(key, values[i]) == 0)
				found = true;
		success &= qstruct_hashmap_iterator_current_value_get(it, value) == QSTRUCT_RESULT_OK;
		success &= strcmp(value, "k") == 0;
	}
	success &= qstruct_hashmap_iterator_destroy(it) == QSTRUCT_RESULT_OK;
	success &= qstruct_hashmap_destroy(map) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_hashmap() {
	bool success = 1;
	success &= test_qstruct_hashmap_add_get();
	success &= test_qstruct_hashmap_has();
	success &= test_qstruct_hashmap_remove();
	success &= test_qstruct_hashmap_iterator();
	return success;
}
