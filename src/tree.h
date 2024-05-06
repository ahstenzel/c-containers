#ifndef CC_STD_TREE_H
#define CC_STD_TREE_H
/**
 * tree.h
 * Unbalanced tree of string nodes.
*/
#include "utilities.h"

#define TREE_FLAG_STR   0b10000000
#ifndef TREE_SERIALIZE_MAX_LEN
#define TREE_SERIALIZE_MAX_LEN 4096
#endif

// Use _Generic to select proper tree creation function
#if __STDC__==1 && __STDC_VERSION >= 201112L

/// @brief Create a new tree.
/// @param t Tree type
/// @return Tree pointer
#define tree_create(t) _Generic((t), \
	char*: _tree_factory(sizeof(t), 1), \
	default: _tree_factory(sizeof(t), 0))

#else

/// @brief Create a new tree.
/// @param t Tree type
/// @return Tree pointer
#define tree_create(t) _tree_factory(sizeof(t), 0)

#endif

/// @brief Create a new tree that stores character strings.
/// @return Tree pointer
#define tree_create_str() _tree_factory(sizeof(char*), 1)

/// @brief Deallocate a tree.
/// @param b Tree pointer
#define tree_destroy(b) _tree_destroy(b);

/// @brief Add a new element to the tree if it does not already exist.
/// @param b Tree pointer
/// @param k Key path
/// @param s Key path seperator
/// @param d Data pointer
/// @return Void data pointer to inserted element, or NULL on failure
#define tree_insert(b, k, s, d) _tree_insert(b, k, s, d)

/// @brief Find the element if it exists in the tree.
/// @param b Tree pointer
/// @param k Key path
/// @param s Key path seperator
/// @return Void data pointer, or NULL if not found
#define tree_find(b, k, s) _tree_find(b, k, s)

/// @brief Remove the element from the tree.
/// @param b Tree pointer
/// @param k Key path
/// @param s Key path seperator
#define tree_delete(b, k, s) _tree_clear(b)

/// @brief Remove all elements from the tree.
/// @param b Tree pointer
#define tree_clear(b) _tree_delete(b, NULL, NULL)

/// @brief Get the depth of a certain node in the tree.
/// @param b Tree pointer
/// @param k Key path
/// @param s Key path seperator
/// @return Node depth
#define tree_depth(b, k, s) _tree_depth(b, k, s)

/// @brief Get the deepest node in the tree.
/// @param b Tree pointer
/// @return Tree depth
#define tree_max_depth(b) _tree_depth(b, NULL, NULL)

/// @brief Get the number of nodes in the tree.
/// @param b Tree pointer
/// @return Tree size
#define tree_length(b) ((b)->_length - 1)

/// @brief Convert the tree and its keys to a human-readable string.
/// @param b Tree pointer
/// @param s Destination string (must be freed later)
#define tree_print(b, s) s = (b) ? (_tree_print((b)->_root, 0)) : NULL

/// @brief Convert the tree and its keys to a serialized (not null-terminated) string.
/// @param b Tree pointer
/// @param s Destination string (must be freed later)
/// @return String length (or 0 on error)
#define tree_serialize(b) _tree_serialize(b, s)

/// @brief Overwrite the tree with the contents of the serialized string.
/// @param b Tree pointer
/// @param s Serialized string
#define tree_deserialize(b, s) _tree_deserialize(b, s)

struct _tree_node_t {
	size_t _num_children;
	struct _tree_node_t** _children;
	void* _buffer;
	char* _key;
};

/// @brief Node in an unbalanced tree.
typedef struct _tree_node_t _tree_node_t;

/// @brief Unbalanced tree of key-value pairs.
typedef struct {
	size_t _element_size;
	size_t _length;
	char _flags;
	_tree_node_t* _root;
} tree_t;

tree_t* _tree_factory(size_t, int);

void _tree_destroy(tree_t*);

void _tree_clear(tree_t*);

void* _tree_find(tree_t*, char*, char*);

void* _tree_insert(tree_t*, char*, char*, void*);

void _tree_delete(tree_t*, char*, char*);

int _tree_depth(tree_t*, char*, char*);

char* _tree_print(_tree_node_t*, size_t);

size_t _tree_serialize(tree_t*, char*);

void _tree_deserialize(tree_t*, char*);

#endif // CC_STD_TREE_H