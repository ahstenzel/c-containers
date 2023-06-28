#ifndef C_UMAP_H
#define C_UMAP_H
/*
 * unordered_map.h
 * Hash table of key-value pairs.
 * Implemented as a simplified Swiss Table architecture.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef _UMAP_32  // 32 bit hash
typedef uint32_t _umap_key_t;
typedef uint32_t _umap_hash_t;
#define _fnv_offset 2166136261U;
#define _fnv_prime 16777619U;
#else // 64 bit hash
typedef uint64_t _umap_key_t;
typedef uint64_t _umap_hash_t;
#define _fnv_offset 14695981039346656037UL;
#define _fnv_prime 1099511628211UL;
#endif

#define _UMAP_DEFAULT_CAPACITY 8
#define _UMAP_DEFAULT_LOAD 0.875f
#define _UMAP_EMPTY 0x80     // 0b1000 0000
#define _UMAP_DELETED 0xFE   // 0b1111 1110
#define _UMAP_SENTINEL 0xFF  // 0b1111 1111


#define _umap_h1(h) h >> 7
#define _umap_h2(h) h & 0x7F
#define _umap_ctrl(u, i) (uint8_t*)(&(u)->_buffer[0] + i)
#define _umap_node(u, i) (&(u)->_buffer[0] + (u)->_capacity + (_umap_node_size((u)->_element_size) * i))
#define _umap_node_key(u, i) (_umap_key_t*)_umap_node(u, i)
#define _umap_node_data(u, i) (void*)(_umap_node(u, i) + ((u)->_element_size > sizeof(_umap_key_t) ? (u)->_element_size : sizeof(_umap_key_t)))

/**
 * Create a new unordered map.
 * @param t Map type
 * @return Map pointer
*/
#define unordered_map_create(t) _umap_factory(sizeof(t), _UMAP_DEFAULT_CAPACITY)

/**
 * Deallocate an unordered map.
 * @param u Map pointer
*/
#define unordered_map_destroy(u) free(u)

/**
 * Add a new element to the map.
 * @param u Map pointer
 * @param k Key
 * @param d Data pointer
 * @return True if element was inserted successfully
*/
#define unordered_map_insert(u, k, d) _umap_insert(&u, k, (void*)d)

/**
 * Find the element if it exists in the map.
 * @param u Map pointer
 * @param k Key
 * @return Void data pointer, or NULL if element does not exist
*/
#define unordered_map_find(u, k) _umap_find(u, k)

/**
 * Remove the element from the map.
 * @param u Map pointer
 * @param k Key
 * @return True if element was removed successfully
*/
#define unordered_map_delete(u, k) _umap_delete(u, k)

/**
 * Get the number of elements in the map.
 * @param u Map pointer
 * @return Map size
*/
#define unordered_map_size(u) (u)->_length

/**
 * Remove all elements from the map.
 * @param u Map pointer
*/
#define unordered_map_clear(u) memset(_umap_ctrl(u, 0), _UMAP_EMPTY, (u)->_capacity)

/**
 * Create an iterator for the map.
 * @param u Map pointer
 * @return Iterator pointer
*/
#define unordered_map_it(u) _umap_it(u)

/**
 * Move the iterator to the next element.
 * @param i Iterator pointer
*/
#define unordered_map_it_next(i) _umap_it_next(&i)

/** Hash table of key-value pairs. */
typedef struct {
	size_t _length;
	size_t _capacity;
	size_t _element_size;
	size_t _load_count;
	uint8_t _buffer[];
} unordered_map;

/** Iterator for an unordered map. */
typedef struct {
	unordered_map* _umap;
	void* data;
	_umap_key_t key;
	size_t _index;
} unordered_map_it_t;

size_t _umap_node_size(size_t);

unordered_map* _umap_factory(size_t, size_t);

unordered_map* _umap_resize(unordered_map*, size_t);

_umap_hash_t _umap_hash(_umap_key_t);

bool _umap_insert(unordered_map**, _umap_key_t, void*);

bool _umap_delete(unordered_map*, _umap_key_t);

void* _umap_find(unordered_map*, _umap_key_t);

unordered_map_it_t* _umap_it(unordered_map*);

void _umap_it_next(unordered_map_it_t**);

#endif  // C_UMAP_H