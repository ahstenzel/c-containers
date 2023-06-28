#ifndef C_PRIORITY_QUEUE_H
#define C_PRIORITY_QUEUE_H
/*
 * priority_queue.h
 * Sorted queue of value-data pairs.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef int32_t priority_queue_value_t;

#define _PRIORITY_QUEUE_DEFAULT_CAPACITY 1

#define _priority_queue_value_pos(q, i) &(q)->_buffer[0] + ((i) * sizeof(priority_queue_value_t))
#define _priority_queue_data_pos(q, i) &(q)->_buffer[0] + ((q)->_capacity * sizeof(priority_queue_value_t)) + ((i) * (q)->_element_size)
#define _priority_queue_value(q, i) *(priority_queue_value_t*)(_priority_queue_value_pos(q, i))

/**
 * Create a new priority queue.
 * @param t Priority queue type
 * @return Priority queue pointer
*/
#define priority_queue_create(t) _priority_queue_factory(sizeof(t), _PRIORITY_QUEUE_DEFAULT_CAPACITY)

/**
 * Deallocate a priority queue.
 * @param q Priority queue pointer
*/
#define priority_queue_destroy(q) free(q)

/**
 * Get the top element in the priority queue.
 * @param q Priority queue pointer
 * @param t Priority queue type
 * @return Void data pointer, or NULL if empty
*/
#define priority_queue_top_data(q) (void*)((q)->_length > 0 ? _priority_queue_data_pos(q, (q)->_length - 1) : NULL)

/**
 * Get the value of the top element in the priority queue.
 * @param q Priority queue pointer
 * @return Value type
*/
#define priority_queue_top_value(q) *(priority_queue_value_t*)((q)->_length > 0 ? _priority_queue_value_pos(q, (q)->_length - 1) : NULL)

/**
 * Add an element to the priority queue.
 * @param q Priority queue pointer
 * @param v Priority value
 * @param d Data pointer
 * @return True if element was added successfully
*/
#define priority_queue_push(q, v, d) _priority_queue_insert(&q, v, (void*)d)

/**
 * Remove the top element from the priority queue.
 * @param q Priority queue pointer
 * @return True if element was removed successfully
*/
#define priority_queue_pop(q) _priority_queue_remove(q, 1)

/**
 * Get the number of elements in the priority queue.
 * @param q Priority queue pointer
 * @return Number of elements
*/
#define priority_queue_size(q) (q)->_length

/**
 * Remove all elements in the priority queue.
 * @param q Priority queue pointer
 * @return True if elements were removed successfully
*/
#define priority_queue_clear(q) _priority_queue_remove(q, (q)->_length)

/**
 * Get the size of the priority queue in memory.
 * @param q Priority queue pointer
 * @return Number of bytes
*/
#define priority_queue_bytes(q) (q) ? (offsetof(priority_queue, _buffer) + _priority_queue_buffer_size((q)->_element_size, (q)->_capacity)) : 

/**
 * Find the element in the priority queue if it exists.
 * @param q Priority queue pointer
 * @param v Priority value
 * @return Data pointer, or NULL if not found
*/
#define priority_queue_find(q, v) (_priority_queue_find(q, v))

/**
 * Create an iterator for the priority queue.
 * @param q Priority queue pointer
 * @return Iterator pointer
*/
#define priority_queue_it(q) _priority_queue_it(q)

/**
 * Move the iterator to the next element.
 * @param i Iterator pointer
*/
#define priority_queue_it_next(i) _priority_queue_it_next(&i)

/** List of elements sorted by priority. */
typedef struct {
	size_t _length;
	size_t _capacity;
	size_t _element_size;
	uint8_t _buffer[];
} priority_queue;

/** Iterator for a priority queue. */
typedef struct {
	priority_queue* _qu;
	void* data;
	size_t _index;
	priority_queue_value_t value;
} priority_queue_it_t;

size_t _priority_queue_buffer_size(size_t, size_t);

priority_queue* _priority_queue_factory(size_t, size_t);

priority_queue* _priority_queue_resize(priority_queue*, size_t);

bool _priority_queue_insert(priority_queue**, priority_queue_value_t, void*);

bool _priority_queue_remove(priority_queue*, size_t);

bool _priority_queue_remove_value(priority_queue*, priority_queue_value_t);

void _priority_queue_sort(priority_queue*);

size_t _priority_queue_find_index(priority_queue*, priority_queue_value_t);

void* _priority_queue_find(priority_queue*, priority_queue_value_t);

priority_queue_it_t* _priority_queue_it(priority_queue*);

void _priority_queue_it_next(priority_queue_it_t**);

#endif	// C_PRIORITY_QUEUE_H