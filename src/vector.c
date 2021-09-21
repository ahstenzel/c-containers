#include "vector.h"

vector* __vec_factory(size_t element_size, size_t capacity) {
  vector* vec = malloc(offsetof(vector, __buffer) + (element_size * capacity));
  if (!vec) { return NULL; }
  vec->length = 0;
  vec->__capacity = capacity;
  vec->__element_size = element_size;
  return vec;
}

vector* __vec_resize(vector* vec, size_t new_capacity) {
  // Create new vector & copy data to it
  vector* new_vec = __vec_factory(vec->__element_size, new_capacity);
  if (!new_vec) { return NULL; }
  memcpy(new_vec->__buffer, vec->__buffer, vec->__element_size * vec->length);
  new_vec->length = vec->length;
  free(vec);
  return new_vec;
}

uint8_t __vec_insert(vector** vec, size_t index, void* data) {
  // Error check
  if (!vec || !(*vec)) { return 1; }
  if (index < 0 || index > ((*vec)->length + 1)) { return 1; }

  // Resize container
  if ((*vec)->length >= (*vec)->__capacity) {
    vector* temp = __vec_resize(*vec, (*vec)->__capacity * 2);
    if (!temp) { return 1; }
    (*vec) = temp;
  }
  
  // Shift over elements
  if (index < (*vec)->length) {
    void* dest = (void*)(&(*vec)->__buffer[0] + (index + 1) * (*vec)->__element_size);
    void* src = (void*)(&(*vec)->__buffer[0] + index * (*vec)->__element_size);
    if (!dest || !src) { return 1; }
    memmove(dest, src, (*vec)->__element_size * ((*vec)->length - index));
  }
  
  // Copy element
  uint8_t* dest = (&(*vec)->__buffer[0] + index * (*vec)->__element_size);
  memcpy(dest, data, (*vec)->__element_size);
  (*vec)->length++;
  return 0;
}

uint8_t __vec_remove(vector* vec, size_t index, size_t count) {
  // Error check
  if (!vec) { return 1; }
  if ((index + count) >= vec->length) { return 1; }
  
  // Shift over elements
  if (index < vec->length) {
    void* src = (void*)(&vec->__buffer[0] + (index + count) * vec->__element_size);
    void* dest = (void*)(&vec->__buffer[0] + index * vec->__element_size);
    if (!dest || !src) { return 1; }
    memmove(dest, src, vec->__element_size * (vec->length - (index + count)));
  }

  // Decrement length
  vec->length -= count;
  return 0;
}