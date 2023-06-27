#include "vector.h"

size_t _vec_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

vector* _vec_factory(size_t element_size, size_t capacity) {
  size_t buffer_size = offsetof(vector, _buffer) + _vec_buffer_size(element_size, capacity);
  vector* vec = malloc(buffer_size);
  if (!vec) { return NULL; }
  memset(vec, 0, buffer_size);
  vec->_capacity = capacity;
  vec->_element_size = element_size;
  return vec;
}

vector* _vec_resize(vector* vec, size_t new_capacity) {
  // Create new vector & copy data to it
  vector* new_vec = _vec_factory(vec->_element_size, new_capacity);
  if (!new_vec) { return NULL; }
  size_t dest_size = vec->_element_size * vec->_length;
  memcpy_s(new_vec->_buffer, dest_size, vec->_buffer, dest_size);
  new_vec->_length = vec->_length;
  free(vec);
  return new_vec;
}

bool _vec_insert(vector** vec, size_t index, void* data) {
  // Error check
  if (!vec || !(*vec)) { return false; }
  if (index < 0 || index > ((*vec)->_length + 1)) { return false; }

  // Resize container
  if ((*vec)->_length >= (*vec)->_capacity) {
    vector* temp = _vec_resize(*vec, (*vec)->_capacity * 2);
    if (!temp) { return false; }
    (*vec) = temp;
  }
  
  // Shift over elements
  vector* _vec = *vec;
  if (index < _vec->_length) {
    void* dest = (void*)(_vec_pos(_vec, index + 1));
    void* src = (void*)(_vec_pos(_vec, index));
    if (!dest || !src) { return false; }
    size_t move_size = _vec->_element_size * (_vec->_length - index);
    memmove_s(dest, move_size, src, move_size);
  }
  
  // Copy element
  uint8_t* dest = _vec_pos(_vec, index);
  size_t dest_size = _vec->_element_size;
  memcpy_s(dest, dest_size, data, dest_size);
  _vec->_length++;
  return true;
}

bool _vec_remove(vector* vec, size_t index, size_t count) {
  // Error check
  if (!vec) { return false; }
  if ((index + count) >= vec->_length) { return false; }
  
  // Shift over elements
  if (index < vec->_length) {
    void* dest = (void*)(_vec_pos(vec, index));
    void* src = (void*)(_vec_pos(vec, index + count));
    if (!dest || !src) { return false; }
    size_t move_size = vec->_element_size * (vec->_length - index - count);
    memmove_s(dest, move_size, src, move_size);
  }

  // Decrement _length
  vec->_length -= count;
  return true;
}