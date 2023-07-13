#ifndef C_DEQUEUE_H
#define C_DEQUEUE_H
/*
 * dequeue.h
 * Double-ended queue.
 */

#include "common.h"

#define _DEQUEUE_DEFAULT_CAPACITY 1

#define _dequeue_pos(q, i) &(q)->_buffer[0] + ((i) * (q)->_element_size)

/**
 * Create a new dequeue.
 * @param t Dequeue type
 * @return Dequeue pointer
*/
#define dequeue_create(t) _dequeue_factory(sizeof(t), _DEQUEUE_DEFAULT_CAPACITY)

/**
 * Deallocate a dequeue.
 * @param q Dequeue pointer
*/
#define dequeue_destroy(q) free(q)

/**
 * Get the front element of the dequeue.
 * @param q Dequeue pointer
 * @return Void data pointer, or NULL if empty
*/
#define dequeue_front(q) (void*)((q)->_length > 0 ? _dequeue_pos(q, (q)->_tail) : NULL)

/**
 * Get the back element of the dequeue.
 * @param q Dequeue pointer
 * @return Void data pointer, or NULL if empty
*/
#define dequeue_back(q) (void*)((q)->_length > 0 ? _dequeue_pos(q, (q)->_head) : NULL)

/**
 * Add an element to the front of the dequeue.
 * @param q Dequeue pointer
 * @param d Data pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define dequeue_push_front(q, d) _dequeue_insert_front(&q, (void*)d)

/**
 * Add an element to the back of the dequeue.
 * @param q Dequeue pointer
 * @param d Data pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define dequeue_push_back(q, d) _dequeue_insert_back(&q, (void*)d)

/**
 * Remove the element at the front of the dequeue.
 * @param q Dequeue pointer
*/
#define dequeue_pop_front(q) _dequeue_remove_front(q, 1)

/**
 * Remove the element at the back of the dequeue.
 * @param q Dequeue pointer
*/
#define dequeue_pop_back(q) _dequeue_remove_back(q, 1)

/**
 * Number of elements in the dequeue.
 * @param q Dequeue pointer
 * @return Dequeue size
*/
#define dequeue_size(q) (q)->_length

/**
 * Remove all elements from the dequeue.
 * @param q Dequeue pointer
*/
#define dequeue_clear(q) _dequeue_remove(q, (q)->_length)

/**
 * Get the size of the dequeue in memory.
 * @param q Dequeue pointer
 * @return Number of bytes
*/
#define dequeue_bytes(q) (q) ? (offsetof(dequeue_t, _buffer) + _dequeue_buffer_size((q)->_element_size, (q)->_capacity)) : 0

/** Double-ended queue. */
typedef struct {
	size_t _length;
	size_t _head;
	size_t _tail;
	size_t _capacity;
	size_t _element_size;
	uint8_t _buffer[];
} dequeue_t;

size_t _dequeue_buffer_size(size_t, size_t);

dequeue_t* _dequeue_factory(size_t, size_t);

dequeue_t* _dequeue_resize(dequeue_t*, size_t);

void* _dequeue_insert_front(dequeue_t**, void*);

void* _dequeue_insert_back(dequeue_t**, void*);

void _dequeue_remove_front(dequeue_t*, size_t);

void _dequeue_remove_back(dequeue_t*, size_t);

#endif	// C_DEQUEUE_H