#include <test/test.h>
#include <qstruct/qstruct.h>

static int8_t _int32_comparator(const char *xp, const char *yp) {
	const int x = *(const int *) xp;
	const int y = *(const int *) yp;
	return x - y;
}

bool test_qstruct_rbtree_add_get() {
	test_run_log("qstruct_rbtree_add_get");
	qstruct_rbtree_t tree;
	int values[] = { 41, 38, 31, 12, 19, 8, 9, 24, 25, 45 };
	size_t value_size = sizeof(int);
	int res_value;

	bool success = 1;
	success &= qstruct_rbtree_create(&tree, &_int32_comparator) == QSTRUCT_RESULT_OK;
	for (int i = 0; i < array_len(values); i++) {
		success &= qstruct_rbtree_add(tree, values + i, value_size) == QSTRUCT_RESULT_OK;
	}
	for (int i = 0; i < array_len(values); i++) {
		res_value = values[i];
		success &= qstruct_rbtree_get(tree, &res_value, &value_size) == QSTRUCT_RESULT_OK;
		success &= res_value == values[i];
	}
	res_value = 0; // A value that dosent exists
	success &= qstruct_rbtree_get(tree, &res_value, &value_size) == QSTRUCT_RESULT_VALUE_NOT_FOUND;
	success &= qstruct_rbtree_length(tree) == array_len(values);
	success &= qstruct_rbtree_destroy(tree) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_rbtree_has() {
	test_run_log("qstruct_rbtree_has");
	qstruct_rbtree_t tree;
	int value = 1;
	size_t value_size = sizeof(int);

	bool success = 1;
	success &= qstruct_rbtree_create(&tree, &_int32_comparator) == QSTRUCT_RESULT_OK;
	success &= !qstruct_rbtree_has(tree, &value);
	success &= qstruct_rbtree_add(tree, &value, value_size) == QSTRUCT_RESULT_OK;
	success &= qstruct_rbtree_has(tree, &value);
	success &= qstruct_rbtree_destroy(tree) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_rbtree_remove() {
	test_run_log("qstruct_rbtree_remove");
	qstruct_rbtree_t tree;
	size_t value_size = sizeof(int);
	int values[] = { 41, 38, 31, 12, 19, 8, 9, 24, 25, 45 };

	bool success = 1;
	success &= qstruct_rbtree_create(&tree, &_int32_comparator) == QSTRUCT_RESULT_OK;
	for (int i = 0; i < array_len(values); i++) {
		success &= qstruct_rbtree_add(tree, values + i, value_size) == QSTRUCT_RESULT_OK;
	}
	for (int i = 0; i < array_len(values); i++) {
		success &= qstruct_rbtree_remove(tree, values + i) == QSTRUCT_RESULT_OK;
		success &= !qstruct_rbtree_has(tree, values + i);
	}
	success &= qstruct_rbtree_length(tree) == 0;
	success &= qstruct_rbtree_destroy(tree) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_rbtree_iterator() {
	test_run_log("qstruct_rbtree_iterator");
	qstruct_rbtree_t tree;
	qstruct_rbtree_iterator_t it;
	int values[] = { 41, 38, 31, 12, 19, 8, 9, 24, 25, 45 };
	size_t value_size = sizeof(int);
	int res_value;

	bool success = 1;
	success &= qstruct_rbtree_create(&tree, &_int32_comparator) == QSTRUCT_RESULT_OK;
	for (int i = 0; i < array_len(values); i++) {
		success &= qstruct_rbtree_add(tree, values + i, value_size) == QSTRUCT_RESULT_OK;
	}
	success &= qstruct_rbtree_iterator_create(tree, &it) == QSTRUCT_RESULT_OK;

	while (qstruct_rbtree_iterator_next(it)) {
		success &= qstruct_rbtree_iterator_current_value(it, &res_value) == QSTRUCT_RESULT_OK;
		bool found = 0;
		for (int i = 0; i < array_len(values) && !found; i++) if (values[i] == res_value) found = true;
		success &= found;
	}

	success &= qstruct_rbtree_iterator_destroy(it) == QSTRUCT_RESULT_OK;
	success &= qstruct_rbtree_destroy(tree) == QSTRUCT_RESULT_OK;
	test_result_log(success);
	return success;
}

bool test_qstruct_rbtree() {
	bool success = 1;
	success &= test_qstruct_rbtree_add_get();
	success &= test_qstruct_rbtree_has();
	success &= test_qstruct_rbtree_remove();
	success &= test_qstruct_rbtree_iterator();
	return success;
}

