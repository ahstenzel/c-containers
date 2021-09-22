/*
 * cstack.h
 * LIFO stack of elements.
 */

#ifndef C_STACK_H
#define C_STACK_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define __STACK_DEFAULT_CAPACITY 8

#define stack_create(t) __stack_factory(sizeof(t), __STACK_DEFAULT_CAPACITY)
#define stack_destroy(s) free(s)
#define stack_head(s, t) *(t*)((s)->length > 0 ? (&(s)->__buffer[0] + ((s)->length - 1) * (s)->__element_size) : NULL)
#define stack_push(s, d) __stack_insert(&s, (void*)d)
#define stack_pop(s) __stack_remove(s, 1)
#define stack_clear(s) __stack_remove(s, (s)->length)
#define stack_max_length(s) 4294967295UL / ((s)->__element_size - 1)
#define stack_bytes(s) offsetof(stack, __buffer) + ((s)->__element_size * (s)->__capacity)

typedef struct {
  size_t length;
  size_t __capacity;
  size_t __element_size;
  uint8_t __buffer[];
} stack;

stack* __stack_factory(size_t, size_t);

stack* __stack_resize(stack*, size_t);

uint8_t __stack_insert(stack**, void*);

uint8_t __stack_remove(stack*, size_t);

#endif  //C_STACK_H