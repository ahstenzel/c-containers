#include "tree.h"
#include "string.h"

_tree_node_t* _tree_find_node(tree_t* tree, _tree_node_t* node, _tree_node_t** parent, size_t* child_idx, int force, char* key, char* sep) {
	char* ctx;
	char* pch = strtok_r(key, sep, &ctx);
	//printf("Finding [%s]\n", key);
	while(pch) {
		int found = 0;
		// Check if any children match the current token
		for(size_t i=0; i<node->_num_children; ++i) {
			_tree_node_t* child = node->_children[i];
			if (strcmp(pch, child->_key) == 0) {
				if (parent && child_idx) {
					(*parent) = node;
					(*child_idx) = i;
				}
				node = child;
				found = 1;
				break;
			}
		}
		if (found == 1) {
			// Get the next token
			pch = strtok_r(NULL, sep, &ctx);
		}
		else {
			// No children match the token
			if (force == 0) { return NULL; }

			// Create a new node
			_tree_node_t* new_node = calloc(1, sizeof *new_node);
			if (!new_node) { return NULL; }

			new_node->_key = strdup(pch);
			if (!new_node->_key) {
				free(new_node);
				return NULL;
			}

			new_node->_buffer = calloc(1, tree->_element_size);
			if (!new_node->_buffer) {
				free(new_node->_key);
				free(new_node);
				return NULL;
			}

			_tree_node_t** new_children = calloc(node->_num_children + 1, sizeof *new_children);
			if (!new_children) {
				free(new_node->_key);
				free(new_node->_buffer);
				free(new_node);
				return NULL; 
			}
			memcpy_s(new_children, sizeof *new_children * (node->_num_children + 1), node->_children, sizeof *new_children * node->_num_children);
			free(node->_children);
			node->_children = new_children;
			node->_children[node->_num_children] = new_node;
			node->_num_children++;
			node = new_node;
			tree->_length++;

			// Get the next token
			pch = strtok_r(NULL, sep, &ctx);
		}
	}
	return node;
}

tree_t* _tree_factory(size_t element_size, int string) {
	tree_t* tree = calloc(1, sizeof *tree);
	if (!tree) { return NULL; }
	tree->_element_size = element_size;
	tree->_root = calloc(1, sizeof *(tree->_root));
	if (!tree->_root) { 
		free(tree);
		return NULL;
	}
	tree->_root->_key = strdup("(root)");
	if (!tree->_root->_key) {
		free(tree->_root);
		free(tree);
		return NULL;
	}
	tree->_length = 1;
	if (string == 1) { tree->_flags |= TREE_FLAG_STR; }
	return tree;
}

void _tree_destroy(tree_t* tree) {
	// Error check
	if (!tree || tree->_length == 0) { return; }
	
	_tree_delete(tree, NULL, NULL);
	free(tree);
	return;
}

void _tree_clear(tree_t* tree) {
	// Error check
	if (!tree || tree->_length == 0) { return; }

	// Clear all elements from the root
	_tree_delete(tree, NULL, NULL);

	// Create a new root
	tree->_root = calloc(1, sizeof *(tree->_root));
	if (!tree->_root) { 
		_tree_destroy(tree);
		return; 
	}
	tree->_length = 1;
	return;
}

void* _tree_find(tree_t* tree, char* key, char* sep) {
	// Error check
	if (!tree || tree->_length == 0) { return NULL; }

	_tree_node_t* node = _tree_find_node(tree, tree->_root, NULL, NULL, 0, key, sep);
	if (!node) { return NULL; }
	return node->_buffer;
}

void* _tree_insert(tree_t* tree, char* key, char* sep, void* data) {
	// Error check
	if (!tree || tree->_length == 0) { return NULL; }

	_tree_node_t* node = _tree_find_node(tree, tree->_root, NULL, NULL, 1, key, sep);
	if (!node) { return NULL; }
	memcpy_s(node->_buffer, tree->_element_size, data, tree->_element_size);
	return node->_buffer;
}

void _tree_delete(tree_t* tree, char* key, char* sep) {
	// Error check
	if (!tree || tree->_length == 0) { return; }

	// Depth-first search to collect all nodes in the subtree
	_tree_node_t* parent = NULL;
	size_t child_idx = 0;
	_tree_node_t* node = (key) ? _tree_find_node(tree, tree->_root, &parent, &child_idx, 0, key, sep) : tree->_root;
	if (!node) { return; }
	_tree_node_t** stack = NULL;
	_tree_node_t** vec = NULL;
	size_t stack_capacity = tree->_length;
	size_t stack_size = 0;
	size_t vec_capacity = stack_capacity;
	size_t vec_size = 0;
	stack = calloc(stack_capacity, sizeof *stack);
	vec = calloc(vec_capacity, sizeof *vec);
	if (!vec || !stack) { goto tree_delete_end; }
	stack[stack_size++] = node;
	while(stack_size > 0) {
		_tree_node_t* v = stack[--stack_size];
		vec[vec_size++] = v;
		for(size_t i=0; i<v->_num_children; ++i) {
			stack[stack_size++] = v->_children[i];
		}
	}

	// Delete all nodes in the subtree
	for(size_t i=0; i<vec_size; ++i) {
		_tree_node_t* v = vec[i];
		free(v->_children);
		free(v->_key);
		free(v->_buffer);
		free(v);
		tree->_length--;
	}
	if (parent) {
		for(size_t i=child_idx; i<parent->_num_children - 1; ++i) {
			parent->_children[i] = parent->_children[i + 1];
		}
		parent->_children[parent->_num_children - 1] = NULL;
		parent->_num_children--;
	}
	
tree_delete_end:
	free(stack);
	free(vec);
	return;
}

int _tree_depth(tree_t* tree, char* key, char* sep) {
	// Error check
	if (!tree || tree->_length == 0) { return -1; }

	// Follow the path through the tree if provided
	if (key) {
		_tree_node_t* node = _tree_find_node(tree, tree->_root, NULL, NULL, 0, key, sep);
		if (!node) { return -1; }
		int count = 0;
		char *pch = key;
		while((pch = strpbrk(pch, sep)) != NULL) {
			count++;
			pch++;
		}
		return count - 1;
	}

	// Breadth-first search to find max depth
	int depth = -1;
	_tree_node_t* node = tree->_root;
	_tree_node_t** queue = NULL;
	size_t queue_capacity = tree->_length;
	size_t queue_head = 0;
	size_t queue_tail = 0;
	queue = calloc(queue_capacity, sizeof *queue);
	if (!queue) { goto tree_depth_end; }

	queue[queue_tail++] = node;
	while((queue_tail - queue_head) > 0) {
		node = queue[queue_head++];
		depth++;
		for(size_t i=0; i<node->_num_children; ++i) {
			queue[queue_tail++] = node->_children[i];
		}
	}

tree_depth_end:
	free(queue);
	return depth;
}

char* _tree_print(_tree_node_t* node, size_t level) {
	// Build string for current node
	if (!node) { return NULL; }
	char* str = NULL;
	size_t key_len = strlen(node->_key);
	size_t len = key_len + (level * 2) + 1;
	str = calloc(len, sizeof *str);
	if (!str) { return NULL; }
	for(size_t i=0; i<level; ++i) {
		if (i == level - 1) {
			strcat_s(str, len, "|-");
		}
		else {
			strcat_s(str, len, "  ");
		}
	}
	strcat_s(str, len, node->_key);

	// Iterate to children
	for(size_t i=0; i<node->_num_children; ++i) {
		char* child = _tree_print(node->_children[i], level + 1);
		size_t child_len = strlen(child);
		size_t cat_len = len + child_len + 1;
		char* cat = calloc(cat_len, sizeof *cat);
		if (!cat) { 
			free(child);
			free(str);
			return NULL; 
		}
		strcat_s(cat, cat_len, str);
		strcat_s(cat, cat_len, "\n");
		strcat_s(cat, cat_len, child);
		free(str);
		free(child);
		str = cat;
		len = cat_len;
	}
	return str;
}

size_t _tree_serialize(tree_t* tree, char* str) {
	// Error check
	if (!tree || tree->_length == 0) { goto tree_serialize_fail; }

	// Build string metadata block
	size_t len = 24;
	str = calloc(TREE_SERIALIZE_MAX_LEN, sizeof *str);
	if (!str) { goto tree_serialize_fail; }
	char* pch = str + 8;
	itoa_s((int)tree_length(tree), pch, TREE_SERIALIZE_MAX_LEN, 10);
	pch += 8;
	itoa_s((int)(tree->_element_size), pch, TREE_SERIALIZE_MAX_LEN, 10);
	pch += 8;
	itoa_s((int)(tree->_flags), pch, TREE_SERIALIZE_MAX_LEN, 10);
	pch += 8;

	// Convert nodes to strings
	_tree_node_t* node = tree->_root;
	_tree_node_t** stack = NULL;
	size_t stack_capacity = tree->_length * 2;
	size_t stack_size = 0;
	stack = calloc(stack_capacity, sizeof *stack);
	if (!stack) { goto tree_serialize_fail; }
	stack[stack_size++] = node;
	while(stack_size > 0) {
		// Get the top node of the stack
		_tree_node_t* v = stack[--stack_size];
		if (!v) {
			// End of nodes children, leave null block
			pch += 4;
			len += 4;
		}
		else {
			// Serialize entire node
			char* nch = pch + 4;
			size_t node_len;
			strcpy_s(nch, TREE_SERIALIZE_MAX_LEN, v->_key);
			node_len = strlen(v->_key) + 1;
			nch += node_len;
			if (tree->_flags & TREE_FLAG_STR) {
				// Copy entire string
				strcpy_s(nch, TREE_SERIALIZE_MAX_LEN, v->_buffer);
				node_len += strlen(v->_buffer) + 1;
			}
			else {
				// Copy buffer data
				memcpy_s(nch, tree->_element_size, v->_buffer, tree->_element_size);
				node_len += tree->_element_size;
			}
			node_len += 4;
			itoa_s((int)node_len, pch, TREE_SERIALIZE_MAX_LEN, 10);

			pch += node_len;
			len += node_len;
		}

		// Add nodes children
		for(size_t i=0; i<v->_num_children; ++i) {
			stack[stack_size++] = v->_children[i];
		}
		stack[stack_size++] = NULL;
	}
	
	// Write final length to the start of the block
	itoa_s((int)len, str, TREE_SERIALIZE_MAX_LEN, 10);
	return len;

tree_serialize_fail:
	free(str);
	return 0;
}

void _tree_deserialize(tree_t* tree, char* str) {

}