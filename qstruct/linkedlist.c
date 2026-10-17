#include <qstruct/qstruct.internal.h>

struct qstruct_linkedlist {
	size_t length;		// length size
	struct entry *entry;	// pointer to first entry
	struct entry *lentry;	// pointer to last entry
};

struct entry {
	struct entry *previous;	// Previous entry
	struct entry *next;	// Next entry
	size_t value_size;	// Size of the value
	uint8_t value[];	// Value of entry
};

/*
 * Gets pointer of entry at given index
 */
static inline qstruct_result_t _ll_getp(qstruct_linkedlist_t ll, struct entry **entry, size_t index) {
	size_t length = ll->length;
	if (length <= index) return QSTRUCT_RESULT_INDEX_OUTOF_BOUND;
	int i;
	struct entry *found_entry;
	if (ll->length / 2 < index) {
		i = length - 1;
		found_entry = ll->lentry;
		while (i != index) {
			found_entry = found_entry->previous;
			i--;
		}
	} else {
		i = 0;
		found_entry = ll->entry;
		while (i != index) {
			found_entry = found_entry->next;
			i++;
		}
	}
	*entry = found_entry;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_create(qstruct_linkedlist_t *list) {
	qstruct_linkedlist_t ll = malloc(sizeof(struct qstruct_linkedlist));
	ll->length = 0;
	ll->entry = NULL;
	ll->lentry = NULL;
	*list = ll;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_add(qstruct_linkedlist_t ll, const void *value, size_t value_size) {
	struct entry *new_entry = malloc(sizeof(struct entry) + value_size);
	if (ll->length == 0) {
		new_entry->previous = NULL;
		new_entry->next = NULL;
		ll->entry = new_entry;
	} else {
		new_entry->previous = ll->lentry;
		new_entry->next = NULL;
		ll->lentry->next = new_entry;
	}
	ll->lentry = new_entry;
	new_entry->value_size = value_size;
	memcpy(new_entry->value, value, value_size);
	ll->length++;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_getp(qstruct_linkedlist_t ll, size_t index, void **value, size_t *value_size) {
	struct entry *entry;
	qstruct_run(_ll_getp(ll, &entry, index));
	*value = entry->value;
	*value_size = entry->value_size;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_get(qstruct_linkedlist_t ll, size_t index, void *value, size_t *value_size) {
	struct entry *e;
	qstruct_run(_ll_getp(ll, &e, index));
	if (*value_size == 0) *value_size = e->value_size;
	if (value != NULL) memcpy(value, e->value, *value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_destroy(qstruct_linkedlist_t ll) {
	qstruct_run(qstruct_linkedlist_clear(ll));
	free(ll);
	return QSTRUCT_RESULT_OK;
}

size_t qstruct_linkedlist_length(qstruct_linkedlist_t ll) {
	return ll->length;
}

qstruct_result_t qstruct_linkedlist_remove(qstruct_linkedlist_t ll, size_t index) {
	struct entry *entry;
	qstruct_run(_ll_getp(ll, &entry, index));
	struct entry *previous = entry->previous;
	struct entry *next = entry->next;

	if (previous == NULL) {
		ll->entry = next;
	} else {
		previous->next = next;
	}

	if (next == NULL) {
		ll->lentry = previous;
	} else {
		next->previous = previous;
	}
	free(entry);
	ll->length--;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_clear(qstruct_linkedlist_t ll) {
	struct entry *entry = ll->entry;
	while (entry != NULL) {
		struct entry *next_entry = entry->next;
		free(entry);
		entry = next_entry;
	}
	ll->entry = NULL;
	ll->lentry = NULL;
	ll->length = 0;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_replace(qstruct_linkedlist_t ll, size_t index, const void *value, size_t value_size) {
	struct entry *entry;
	qstruct_run(_ll_getp(ll, &entry, index));
	if (entry->value_size != value_size) {
		struct entry *old_entry = entry;
		entry = realloc(entry, sizeof(struct entry) + value_size);
		if (old_entry != entry) {
			if (entry->previous != NULL) {
				entry->previous->next = entry;
			} else {
				ll->entry = entry;
			}
			if (entry->next != NULL) {
				entry->next->previous = entry;
			} else {
				ll->lentry = entry;
			}
		}
	}
	entry->value_size = value_size;
	memcpy(entry->value, value, value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_linkedlist_insert(qstruct_linkedlist_t ll, size_t index, const void *value, size_t value_size) {
	struct entry *entry;
	qstruct_run(_ll_getp(ll, &entry, index));
	struct entry *previous_entry = entry->previous;
	struct entry *new_entry = malloc(sizeof(struct entry) + value_size);
	new_entry->value_size = value_size;
	memcpy(new_entry->value, value, value_size);
	previous_entry->next = new_entry;
	new_entry->next = entry;
	entry->previous = new_entry;
	ll->length++;
	return QSTRUCT_RESULT_OK;
}

