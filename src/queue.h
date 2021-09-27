/*
 * queue.h
 * FIFO group of elements.
 */

#ifndef C_QUEUE_H
#define C_QUEUE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define __QUEUE_DEFAULT_CAPACITY 32

#define __queue_pos(q, i) &(q)->__buffer[0] + (i * (q)->__element_size)

#define queue_create(t) __queue_factory(sizeof(t), __QUEUE_DEFAULT_CAPACITY)
#define queue_destroy(q) free(q)
#define queue_head(q, t) *(t*)((q)->length > 0 ? __queue_pos(q, (q)->__head) : NULL)
#define queue_push(q, d) __queue_insert(&q, (void*)d)
#define queue_pop(q) __queue_remove(q, 1)
#define queue_clear(q) __queue_remove(q, (q)->length)
#define queue_bytes(q) (q) ? offsetof(queue, __buffer) + __queue_buffer_size((q)->__element_size, (q)->__capacity)) : 0

typedef struct {
  size_t length;
  size_t __head;
  size_t __tail;
  size_t __capacity;
  size_t __element_size;
  uint8_t __buffer[];
} queue;

size_t __queue_buffer_size(size_t, size_t);

queue* __queue_factory(size_t, size_t);

queue* __queue_resize(queue*, size_t);

uint8_t __queue_insert(queue**, void*);

uint8_t __queue_remove(queue*, size_t);

#endif