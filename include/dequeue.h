#ifndef C_DEQUEUE_H
#define C_DEQUEUE_H
/*
 * dequeue.h
 * Double-ended queue.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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
 * @param t Dequeue type
 * @return Element at front casted to type t
*/
#define dequeue_front(q, t) *(t*)((q)->_length > 0 ? _dequeue_pos(q, (q)->_tail) : NULL)

/**
 * Get the back element of the dequeue.
 * @param q Dequeue pointer
 * @param t Dequeue type
 * @return Element at back casted to type t
*/
#define dequeue_back(q, t) *(t*)((q)->_length > 0 ? _dequeue_pos(q, (q)->_head) : NULL)

/**
 * Add an element to the front of the dequeue.
 * @param q Dequeue pointer
 * @param d Data pointer
 * @return True if element was added successfully
*/
#define dequeue_push_front(q, d) _dequeue_insert_front(&q, (void*)d)

/**
 * Add an element to the back of the dequeue.
 * @param q Dequeue pointer
 * @param d Data pointer
 * @return True if element was added successfully
*/
#define dequeue_push_back(q, d) _dequeue_insert_back(&q, (void*)d)

/**
 * Remove the element at the front of the dequeue.
 * @param q Dequeue pointer
 * @return True if element was removed successfully
*/
#define dequeue_pop_front(q) _dequeue_remove_front(q, 1)

/**
 * Remove the element at the back of the dequeue.
 * @param q Dequeue pointer
 * @return True if element was removed successfully
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
 * @return True if elements removed successfully
*/
#define dequeue_clear(q) _dequeue_remove(q, (q)->_length)

/**
 * Get the size of the dequeue in memory.
 * @param q Dequeue pointer
 * @return Number of bytes
*/
#define dequeue_bytes(q) (q) ? (offsetof(dequeue, _buffer) + _dequeue_buffer_size((q)->_element_size, (q)->_capacity)) : 0

/** Double-ended queue. */
typedef struct {
	size_t _length;
	size_t _head;
	size_t _tail;
	size_t _capacity;
	size_t _element_size;
	uint8_t _buffer[];
} dequeue;

size_t _dequeue_buffer_size(size_t, size_t);

dequeue* _dequeue_factory(size_t, size_t);

dequeue* _dequeue_resize(dequeue*, size_t);

bool _dequeue_insert_front(dequeue**, void*);

bool _dequeue_insert_back(dequeue**, void*);

bool _dequeue_remove_front(dequeue*, size_t);

bool _dequeue_remove_back(dequeue*, size_t);

#endif