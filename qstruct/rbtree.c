#include <qstruct/qstruct.internal.h>
#include <stdbool.h>

#define IS_RED(n) (n != NULL && n->red)

struct qstruct_rbtree {
	qstruct_rbtree_comparator_t comparator;
	struct node *root;
	size_t length;
};

struct node {
	struct node *parent;
	struct node *right;
	struct node *left;
	bool red;
	size_t value_size;
	char value[];
};

struct qstruct_rbtree_iterator {
	qstruct_rbtree_t tree;
	size_t size;
	int index;
	struct node *nodes[];
};

/*
 * Performes a right rotate
 */
static inline void _rbt_right_rotate(qstruct_rbtree_t t, struct node *n) {
	struct node *l = n->left;

	n->left = l->right;
	if (l->right != NULL) {
		l->right->parent = n;
	}

	l->parent = n->parent;
	if (n->parent == NULL) {
		t->root = l;
	} else if (n->parent->right == n) {
		n->parent->right = l;
	} else {
		n->parent->left = l;
	}

	l->right = n;
	n->parent = l;
}

/*
 * Performes a left rotate
 */
static inline void _rbt_left_rotate(qstruct_rbtree_t t, struct node *n) {
	struct node *l = n->right;

	n->right = l->left;
	if (l->left != NULL) {
		l->left->parent = n;
	}

	l->parent = n->parent;
	if (n->parent == NULL) {
		t->root = l;
	} else if (n->parent->right == n) {
		n->parent->right = l;
	} else {
		n->parent->left = l;
	}

	l->left = n;
	n->parent = l;
}

/*
 * Fix tree after add action
 */
static inline void _rbt_fix_add(qstruct_rbtree_t t, struct node *n) {
	while (IS_RED(n->parent)) {
		struct node *p = n->parent;
		struct node *g = p->parent;
		struct node *u;
		if (g->left == p) {
			u = g->right;
			if (IS_RED(u)) {
				goto _rbt_fix_add_recolor;
			} else {
				if (n == p->right) {
					n = p;
					p = n->parent;
					g = p->parent;
					_rbt_left_rotate(t, n);
				}
				p->red = false;
				g->red = true;
				_rbt_right_rotate(t, g);
			}
		} else {
			u = g->left;
			if (IS_RED(u)) {
				goto _rbt_fix_add_recolor;
			} else {
				if (n == p->left) {
					n = p;
					p = n->parent;
					g = p->parent;
					_rbt_right_rotate(t, n);
				}
				p->red = false;
				g->red = true;
				_rbt_left_rotate(t, g);
			}
		}
		continue;
_rbt_fix_add_recolor:
		p->red = false;
		u->red = false;
		g->red = true;
		n = g;
		continue;
	}
	t->root->red = false;
}

/*
 * Finds a node and return a pointer to it
 * Returns NULL if node not found
 */
struct node *_rbt_find_node(qstruct_rbtree_t t, const void *value) {
	qstruct_rbtree_comparator_t comparator = t->comparator;
	struct node *n = t->root;
	while (n != NULL) {
		int8_t cres = comparator(value, n->value);
		if (cres == 0) break;
		else if (cres < 0) n = n->right;
		else n = n->left;
	}
	return n;
}

qstruct_result_t qstruct_rbtree_create(qstruct_rbtree_t *tree, qstruct_rbtree_comparator_t comparator) {
	qstruct_rbtree_t t = malloc(sizeof(struct qstruct_rbtree));
	t->comparator = comparator;
	t->root = NULL;
	t->length = 0;
	*tree = t;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_rbtree_add(qstruct_rbtree_t t, const void *value, size_t value_size) {
	qstruct_rbtree_comparator_t comparator = t->comparator;

	struct node *current = t->root;
	struct node *parent = current;
	bool bigchild;
	while (current != NULL) {
		parent = current;
		int8_t cres = comparator(value, current->value);
		bigchild = cres > 0;
		if (cres == 0) {
			memcpy(current->value, value, value_size);
			return QSTRUCT_RESULT_OK;
		} else if (cres < 0) {
			current = current->right;
		} else {
			current = current->left;
		}
	}

	struct node *new_node = malloc(sizeof(struct node) + value_size);
	new_node->red = true;
	new_node->parent = parent;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->value_size = value_size;
	memcpy(new_node->value, value, value_size);

	if (parent == NULL) {
		t->root = new_node;
	} else if (bigchild) {
		parent->left = new_node;
	} else {
		parent->right = new_node;
	}

	_rbt_fix_add(t, new_node);
	t->length++;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_rbtree_get(qstruct_rbtree_t t, void *value, size_t *value_size) {
	void *src = value;
	size_t fetched_value_size;
	qstruct_run(qstruct_rbtree_getp(t, &value, &fetched_value_size));
	if (*value_size == 0) *value_size = fetched_value_size;
	if (value != NULL) memcpy(value, src, *value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_rbtree_getp(qstruct_rbtree_t t, void **value, size_t *value_size) {
	struct node *n = _rbt_find_node(t, *value);
	if (n == NULL) return QSTRUCT_RESULT_VALUE_NOT_FOUND;
	if (value_size) *value_size = n->value_size;
	*value = n->value;
	return QSTRUCT_RESULT_OK;
}

bool qstruct_rbtree_has(qstruct_rbtree_t tree, const void *value) {
	qstruct_rbtree_t t = tree;
	return _rbt_find_node(t, value) != NULL;
}

/*
 * Finds successor of the node
 */
inline static struct node* _rbt_get_successor(struct node *n) {
	if (n->left == NULL && n->right == NULL) return NULL;
	if (n->left != NULL && n->right == NULL) return n->left;
	if (n->left == NULL && n->right != NULL) return n->right;
	struct node *s = n->right;
	while (s->left != NULL) s = s->left;
	return s;
}

/*
 * Fixes double black after removal
 */
static inline void _rbt_fix_double_black(qstruct_rbtree_t t, struct node *n) {
	if (n == t->root) return;
	struct node *p = n->parent;
	struct node *s = n == p->left ? p->right : p->left;
	if (s == NULL) {
		_rbt_fix_double_black(t, p);
	} else {
		if (IS_RED(s)) {
			p->red = true;
			s->red = false;
			if (s == p->left)
				_rbt_right_rotate(t, p);
			else
				_rbt_left_rotate(t, p);
			_rbt_fix_double_black(t, n);
		} else {
			if (IS_RED(s->left) || IS_RED(s->right)) {
				if (IS_RED(s->left)) {
					if (s == p->left) {
						s->left->red = s->red;
						s->red = p->red;
						_rbt_right_rotate(t, p);
					} else {
						s->left->red = p->red;
						_rbt_right_rotate(t, s);
						_rbt_left_rotate(t, p);
					}
				} else {
					if (s == p->left) {
						s->right->red = p->red;
						_rbt_left_rotate(t, s);
						_rbt_right_rotate(t, p);
					} else {
						s->right->red = s->red;
						s->red = p->red;
						_rbt_left_rotate(t, p);
					}
				}
				p->red = false;
			} else {
				s->red = true;
				if (!IS_RED(p))
					_rbt_fix_double_black(t, p);
				else
					p->red = false;
			}
		}
	}
}

/*
 * Replace node n possition with s
 * Doesnt copy value or free node n
 */
static inline void _rbt_replace_node_novalue(struct node *n, struct node *s) {
	s->parent = n->parent;
	s->left = n->left;
	s->right = n->right;
	s->red = n->red;

	if (s->right != NULL) s->right->parent = s;
	if (s->left != NULL) s->left->parent = s;

	if (n->parent != NULL) {
		if (n == n->parent->left) n->parent->left = s;
		else n->parent->right = s;
	}
}

/*
 * Resize node but doesnt copy value to new created node
 */
static inline void _rbt_resize_node_novalue(struct node **n, size_t size) {
	struct node *ln = *n;
	if (ln->value_size == size) return;
	struct node *nln = malloc(sizeof(struct node) + size);
	_rbt_replace_node_novalue(ln, nln);
	free(ln);
	nln->value_size = size;
	*n = ln;
}

/*
 * Swap values of given nodes
 */
static inline void _rbt_swap_value(struct node **v, struct node **u) {
	struct node* lv = *v;
	struct node* lu = *u;
	if (lv->value_size == lu->value_size) {
		char temp[lv->value_size];
		memcpy(temp, lv->value, lv->value_size);
		memcpy(lv->value, lu->value, lv->value_size);
		memcpy(lu->value, temp, lu->value_size);
	} else {
		struct node* nv = malloc(sizeof(struct node) + lu->value_size);
		struct node* nu = malloc(sizeof(struct node) + lv->value_size);
		memcpy(nv->value, lu->value, nv->value_size);
		memcpy(nu->value, lv->value, nu->value_size);
		_rbt_replace_node_novalue(lv, nv);
		_rbt_replace_node_novalue(lu, nu);
		free(lv);
		free(lu);
		*v = nv;
		*u = nu;
	}
}

/*
 * Sets value of u to v
 * And returns new v
 */
static inline struct node* _rbt_set_value(struct node *v, struct node *u) {
	_rbt_resize_node_novalue(&v, u->value_size);
	memcpy(v->value, u->value, v->value_size);
	return v;
}

/*
 * Removes given node from the tree
 */
static inline void _rbt_remove_node(qstruct_rbtree_t t, struct node *v) {
	struct node *u = _rbt_get_successor(v);
	bool uv_black = !IS_RED(u) && !IS_RED(v);
	struct node *p = v->parent;

	if (u == NULL) {
		if (p == NULL) {
			t->root = NULL;
		} else {
			if (uv_black) {
				_rbt_fix_double_black(t, v);
			} else {
				struct node *s = v == p->right ? p->left : p->right;
				if (s != NULL) {
					s->red = true;
				}
			}

			if (v == p->right)
				p->right = NULL;
			else
				p->left = NULL;
		}
		free(v);
		return;
	}

	if (v->left == NULL || v->right == NULL) {
		if (p == NULL) {
			t->root = u;
			u->parent = NULL;
			free(v);
		} else {
			if (v == p->left)
				p->left = u;
			else
				p->right = u;
			free(v);
			u->parent = p;
			if (uv_black) _rbt_fix_double_black(t, u);
			else u->red = false;
		}
		return;
	}

	_rbt_swap_value(&v, &u);
	_rbt_remove_node(t, u);
}

qstruct_result_t qstruct_rbtree_remove(qstruct_rbtree_t t, const void *value) {
	struct node *n = _rbt_find_node(t, value);
	if (n == NULL) return QSTRUCT_RESULT_VALUE_NOT_FOUND;
	_rbt_remove_node(t, n);
	t->length--;
	return QSTRUCT_RESULT_OK;
}

/*
 * Destroy given node and all of its children recurivly
 */
static inline void _rbt_destroy(struct node *n) {
	if (n == NULL) return;
	_rbt_destroy(n->right);
	_rbt_destroy(n->left);
	free(n);
}

qstruct_result_t qstruct_rbtree_destroy(qstruct_rbtree_t t) {
	_rbt_destroy(t->root);
	free(t);
	return QSTRUCT_RESULT_OK;
}

size_t qstruct_rbtree_length(qstruct_rbtree_t tree) {
	qstruct_rbtree_t t = tree;
	return t->length;
}

/*
 * Add node and all of its children to iterator
 * (make sure iterator has enough capacity)
 */
static inline void _rbt_iter_add_node(qstruct_rbtree_iterator_t it, struct node *n) {
	if (n == NULL) return;
	it->nodes[it->size++] = n;
	_rbt_iter_add_node(it, n->left);
	_rbt_iter_add_node(it, n->right);
}

qstruct_result_t qstruct_rbtree_iterator_create(qstruct_rbtree_t t, qstruct_rbtree_iterator_t *iterator) {
	qstruct_rbtree_iterator_t it = malloc(sizeof(struct qstruct_rbtree_iterator) + sizeof(struct node*) * t->length);
	it->tree = t;
	it->size = 0;
	it->index = -1;
	_rbt_iter_add_node(it, t->root);
	*iterator = it;
	return QSTRUCT_RESULT_OK;
}

bool qstruct_rbtree_iterator_next(qstruct_rbtree_iterator_t it) {
	if (it->index + 1 >= it->size) return false;
	it->index++;
	return true;
}

size_t qstruct_rbtree_iterator_current_size(qstruct_rbtree_iterator_t it) {
	struct node *node = it->nodes[it->index];
	return node->value_size;
}

qstruct_result_t qstruct_rbtree_iterator_current_value(qstruct_rbtree_iterator_t it, void *buffer) {
	struct node *node = it->nodes[it->index];
	memcpy(buffer, node->value, node->value_size);
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_rbtree_iterator_current_valuep(qstruct_rbtree_iterator_t it, void **buffer) {
	struct node *node = it->nodes[it->index];
	*buffer = node->value;
	return QSTRUCT_RESULT_OK;
}

qstruct_result_t qstruct_rbtree_iterator_destroy(qstruct_rbtree_iterator_t it) {
	free(it);
	return QSTRUCT_RESULT_OK;
}

