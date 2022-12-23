/*
 * stack.h
 * LIFO group of elements.
 */

#ifndef C_STACK_H
#define C_STACK_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define __STACK_DEFAULT_CAPACITY 1

#define __stack_pos(s, i) &(s)->__buffer[0] + ((i) * (s)->__element_size)

#define stack_create(t) __stack_factory(sizeof(t), __STACK_DEFAULT_CAPACITY)
#define stack_destroy(s) free(s)
#define stack_head(s, t) *(t*)((s)->__length > 0 ? __stack_pos(s, (s)->__length - 1) : NULL)
#define stack_push(s, d) __stack_insert(&s, (void*)d)
#define stack_pop(s) __stack_remove(s, 1)
#define stack_size(s) (s)->__length
#define stack_clear(s) __stack_remove(s, (s)->__length)
#define stack_bytes(s) (s) ? (offsetof(stack, __buffer) + __stack_buffer_size((s)->__element_size, (s)->__capacity)) : 0

typedef struct {
  size_t __length;
  size_t __capacity;
  size_t __element_size;
  uint8_t __buffer[];
} stack;

size_t __stack_buffer_size(size_t, size_t);

stack* __stack_factory(size_t, size_t);

stack* __stack_resize(stack*, size_t);

uint8_t __stack_insert(stack**, void*);

uint8_t __stack_remove(stack*, size_t);

#endif  //C_STACK_H