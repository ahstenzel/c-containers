#ifndef C_QUEUE_H
#define C_QUEUE_H
/*
 * queue.h
 * FIFO group of elements.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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
 * @param t Queue type
 * @return Queue head casted to type t
*/
#define queue_head(q, t) *(t*)((q)->_length > 0 ? _queue_pos(q, (q)->_head) : NULL)

/**
 * Add a new element to the back of the queue.
 * @param q Queue pointer
 * @return True if element was added successfully
*/
#define queue_push(q, d) _queue_insert(&q, (void*)d)

/**
 * Remove the element at the front of the queue.
 * @param q Queue pointer
 * @return True if element was removed successfully
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
} queue;

size_t _queue_buffer_size(size_t, size_t);

queue* _queue_factory(size_t, size_t);

queue* _queue_resize(queue*, size_t);

bool _queue_insert(queue**, void*);

bool _queue_remove(queue*, size_t);

#endif