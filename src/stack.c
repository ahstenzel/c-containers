#include "stack.h"

size_t __stack_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

stack* __stack_factory(size_t element_size, size_t capacity) {
  stack* stk = malloc(offsetof(stack, __buffer) + __stack_buffer_size(element_size, capacity));
  if (!stk) { return NULL; }
  stk->__length = 0;
  stk->__capacity = capacity;
  stk->__element_size = element_size;
  return stk;
}

stack* __stack_resize(stack* stk, size_t new_capacity) {
  // Create new stack & copy data to it
  stack* new_stk = __stack_factory(stk->__element_size, new_capacity);
  if (!new_stk) { return NULL; }
  memcpy(new_stk->__buffer, stk->__buffer, stk->__element_size * stk->__length);
  new_stk->__length = stk->__length;
  free(stk);
  return new_stk;
}

uint8_t __stack_insert(stack** stk, void* data) {
  // Error check
  if (!stk || !(*stk)) { return 1; }

  // Resize container
  if ((*stk)->__length >= (*stk)->__capacity) {
    stack* temp = __stack_resize(*stk, (*stk)->__capacity * 2);
    if (!temp) { return 1; }
    (*stk) = temp;
  }

  // Copy element
  void* dest = (void*)(__stack_pos(*stk, (*stk)->__length));
  memcpy(dest, data, (*stk)->__element_size);
  (*stk)->__length++;
  return 0;
}

uint8_t __stack_remove(stack* stk, size_t count) {
  // Error check
  if (!stk) { return 1; }

  // Decrement __length
  stk->__length -= count;
  return 0;
}