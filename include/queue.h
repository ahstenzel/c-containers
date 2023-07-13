#ifndef C_QUEUE_H
#define C_QUEUE_H
/*
 * queue.h
 * FIFO group of elements.
 */

#include "common.h"

#define _QUEUE_DEFAULT_CAPACITY 1

#define _queue_pos(q, i) &(q)->_buffer[0] + ((i) * (q)->_element_size)

/**
 * Create a new queue.
 * @param t Queue type
 * @return Queue pointer
*/
#define queue_create(t) _queue_factory(sizeof(t), _QUEUE_DEFAULT_CAPACITY)

/**
 * Destroy the queue.
 * @param q Queue pointer
*/
#define queue_destroy(q) free(q)

/**
 * Get the front element of the queue.
 * @param q Queue pointer
 * @return Void data pointer, or NULL if empty
*/
#define queue_head(q) (void*)((q)->_length > 0 ? _queue_pos(q, (q)->_head) : NULL)

/**
 * Add a new element to the back of the queue.
 * @param q Queue pointer
 * @return Void data pointer to inserted element, or NULL
*/
#define queue_push(q, d) _queue_insert(&q, (void*)d)

/**
 * Remove the element at the front of the queue.
 * @param q Queue pointer
*/
#define queue_pop(q) _queue_remove(q, 1)

/**
 * Get the number of elements in the queue.
 * @param q Queue pointer
 * @return Queue size
*/
#define queue_size(q) (q)->_length

/**
 * Remove all elements from the queue.
 * @param q Queue pointer
 * @return True if elements were removed successfully
*/
#define queue_clear(q) _queue_remove(q, (q)->_length)

/**
 * Get the size of the queue in memory.
 * @param q Queue pointer
 * @return Number of bytes
*/
#define queue_bytes(q) (q) ? (offsetof(queue_t, _buffer) + _queue_buffer_size((q)->_element_size, (q)->_capacity)) : 0

/** FIFO group of elements. */
typedef struct {
	size_t _length;
	size_t _head;
	size_t _tail;
	size_t _capacity;
	size_t _element_size;
	uint8_t _buffer[];
} queue_t;

size_t _queue_buffer_size(size_t, size_t);

queue_t* _queue_factory(size_t, size_t);

queue_t* _queue_resize(queue_t*, size_t);

void* _queue_insert(queue_t**, void*);

void _queue_remove(queue_t*, size_t);

#endif	// C_QUEUE_H