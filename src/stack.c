#include "stack.h"

stack* __stack_factory(size_t element_size, size_t capacity) {
  stack* stk = malloc(offsetof(stack, __buffer) + (element_size * capacity));
  if (!stk) { return NULL; }
  stk->length = 0;
  stk->__capacity = capacity;
  stk->__element_size = element_size;
  return stk;
}

stack* __stack_resize(stack* stk, size_t new_capacity) {
  // Create new stack & copy data to it
  stack* new_stk = __stack_factory(stk->__element_size, new_capacity);
  if (!new_stk) { return NULL; }
  memcpy(new_stk->__buffer, stk->__buffer, stk->__element_size * stk->length);
  new_stk->length = stk->length;
  free(stk);
  return new_stk;
}

uint8_t __stack_insert(stack** stk, void* data) {
  // Error check
  if (!stk || !(*stk)) { return 1; }

  // Resize container
  if ((*stk)->length >= (*stk)->__capacity) {
    stack* temp = __stack_resize(*stk, (*stk)->__capacity * 2);
    if (!temp) { return 1; }
    (*stk) = temp;
  }

  // Copy element
  void* dest = (void*)(&(*stk)->__buffer[0] + (*stk)->length * (*stk)->__element_size);
  memcpy(dest, data, (*stk)->__element_size);
  (*stk)->length++;
  return 0;
}

uint8_t __stack_remove(stack* stk, size_t count) {
  // Error check
  if (!stk) { return 1; }

  // Decrement length
  stk->length -= count;
  return 0;
}