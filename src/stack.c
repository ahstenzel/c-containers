#include "stack.h"

size_t _stack_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

stack* _stack_factory(size_t element_size, size_t capacity) {
  stack* stk = malloc(offsetof(stack, _buffer) + _stack_buffer_size(element_size, capacity));
  if (!stk) { return NULL; }
  stk->_length = 0;
  stk->_capacity = capacity;
  stk->_element_size = element_size;
  return stk;
}

stack* _stack_resize(stack* stk, size_t new_capacity) {
  // Create new stack & copy data to it
  stack* new_stk = _stack_factory(stk->_element_size, new_capacity);
  if (!new_stk) { return NULL; }
  memcpy(new_stk->_buffer, stk->_buffer, stk->_element_size * stk->_length);
  new_stk->_length = stk->_length;
  free(stk);
  return new_stk;
}

bool _stack_insert(stack** stk, void* data) {
  // Error check
  if (!stk || !(*stk)) { return false; }

  // Resize container
  if ((*stk)->_length >= (*stk)->_capacity) {
    stack* temp = _stack_resize(*stk, (*stk)->_capacity * 2);
    if (!temp) { return false; }
    (*stk) = temp;
  }

  // Copy element
  void* dest = (void*)(_stack_pos(*stk, (*stk)->_length));
  memcpy(dest, data, (*stk)->_element_size);
  (*stk)->_length++;
  return true;
}

bool _stack_remove(stack* stk, size_t count) {
  // Error check
  if (!stk) { return false; }

  // Decrement _length
  stk->_length -= count;
  return true;
}