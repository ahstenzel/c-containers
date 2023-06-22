#ifndef C_STACK_H
#define C_STACK_H
/*
 * stack.h
 * LIFO group of elements.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define _STACK_DEFAULT_CAPACITY 1

#define _stack_pos(s, i) &(s)->_buffer[0] + ((i) * (s)->_element_size)

/**
 * Create a new stack.
 * @param t Stack type
 * @return Stack pointer
*/
#define stack_create(t) _stack_factory(sizeof(t), _STACK_DEFAULT_CAPACITY)

/**
 * Deallocate a stack.
 * @param s Stack pointer
*/
#define stack_destroy(s) free(s)

/**
 * Get the top element of the stack.
 * @param s Stack pointer
 * @param t Stack type
 * @return Stack head casted to type t
*/
#define stack_head(s, t) *(t*)((s)->_length > 0 ? _stack_pos(s, (s)->_length - 1) : NULL)

/**
 * Add a new element to the top of the stack.
 * @param s Stack pointer
 * @param d Data pointer
 * @return True if element was added successfully
*/
#define stack_push(s, d) _stack_insert(&s, (void*)d)

/**
 * Remove the element from the top of the stack.
 * @param s Stack pointer
 * @return True if element was removed successfully
*/
#define stack_pop(s) _stack_remove(s, 1)

/**
 * Get the number of elements in the stack.
 * @param s Stack pointer
 * @return Stack size
*/
#define stack_size(s) (s)->_length

/**
 * Remove all elements from the stack.
 * @param s Stack pointer
 * @return True if elements were removed successfully
*/
#define stack_clear(s) _stack_remove(s, (s)->_length)

/**
 * Get the size of the stack in memory.
 * @param s Stack pointer
 * @return Number of bytes
*/
#define stack_bytes(s) (s) ? (offsetof(stack_t, _buffer) + _stack_buffer_size((s)->_element_size, (s)->_capacity)) : 0

/** LIFO group of elements. */
typedef struct {
  size_t _length;
  size_t _capacity;
  size_t _element_size;
  uint8_t _buffer[];
} stack;

size_t _stack_buffer_size(size_t, size_t);

stack* _stack_factory(size_t, size_t);

stack* _stack_resize(stack*, size_t);

bool _stack_insert(stack**, void*);

bool _stack_remove(stack*, size_t);

#endif  //C_STACK_H