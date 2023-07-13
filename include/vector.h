#ifndef C_VECTOR_H
#define C_VECTOR_H
/*
 * vector.h
 * Dynamically resizing array.
 */

#include "common.h"

#define _VECTOR_DEFAULT_CAPACITY 1

#define _vec_pos(v, i) &(v)->_buffer[0] + ((i) * (v)->_element_size)

/**
 * Create a new vector.
 * @param t Vector type
 * @return Vector pointer
*/
#define vector_create(t) _vec_factory(sizeof(t), _VECTOR_DEFAULT_CAPACITY)

/**
 * Deallocate a vector.
 * @param v Vector pointer
*/
#define vector_destroy(v) free(v)

/**
 * Get an element from the vector.
 * @param v Vector pointer
 * @param i Index
 * @result Void data pointer, or NULL if out of range
*/
#define vector_get(v, i) (void*)((i < (v)->_length && i >= 0) ? _vec_pos(v, i) : NULL)

/**
 * Set the element in the vector.
 * @param v Vector pointer
 * @param i Index
 * @param d Data pointer
*/
#define vector_set(v, i, d) ({ if (i < (v)->_length && i >= 0) memcpy_s(_vec_pos(v, i), (v)->_element_size, d, (v)->_element_size) })

/**
 * Get the number of elements in the vector.
 * @param v Vector pointer
 * @return Vector size
*/
#define vector_size(v) (v)->_length

/**
 * Add an element to the end of the vector.
 * @param v Vector pointer
 * @param d Data pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define vector_push_back(v, d) _vec_insert(&v, (v)->_length, (void*)d)

/**
 * Add an element to the front of the vector.
 * @param v Vector pointer
 * @param d Data pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define vector_push_front(v, d) _vec_insert(&v, 0, (void*)d)

/**
 * Remove the element at the end of the vector.
 * @param v Vector pointer
*/
#define vector_pop_back(v) _vec_remove(v, (v)->_length - 1, 1)

/**
 * Remove the element at the front of the vector.
 * @param v Vector pointer
*/
#define vector_pop_front(v) _vec_remove(v, 0, 1)

/**
 * Insert the element at the point in the vector.
 * @param v Vector pointer
 * @param i Index
 * @param d Data pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define vector_insert(v, i, d) _vec_insert(v, i, (void*)d)

/**
 * Remove the element at the point in the vector.
 * @param v Vector pointer
 * @param i Index
*/
#define vector_remove(v, i) _vec_remove(v, i, 1)

/**
 * Remove all elements from the vector.
 * @param v Vector pointer
*/
#define vector_clear(v) _vec_remove(v, 0, (v)->_length)

/**
 * Get the size of the vector in memory.
 * @param v Vector pointer
 * @return Number of bytes
*/
#define vector_bytes(v) (v) ? (offsetof(vector_t, _buffer) + _vec_buffer_size((v)->_element_size, (v)->_capacity)) : 0

/** Dynamically resizing array. */
typedef struct {
	size_t _length;
	size_t _capacity;
	size_t _element_size;
	uint8_t _buffer[];
} vector_t;

size_t _vec_buffer_size(size_t, size_t);

vector_t* _vec_factory(size_t, size_t);

vector_t* _vec_resize(vector_t*, size_t);

void* _vec_insert(vector_t**, size_t, void*);

void _vec_remove(vector_t*, size_t, size_t);


#endif  // C_VECTOR_H