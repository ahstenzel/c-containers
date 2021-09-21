/*
 * cvector.h
 * Dynamically resizing array.
 */

#ifndef C_VECTOR_H
#define C_VECTOR_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define __VECTOR_DEFAULT_CAPACITY 8

#define vector_create(t) __vec_factory(sizeof(t), __VECTOR_DEFAULT_CAPACITY)
#define vector_destroy(v) free(v)
#define vector_get(v, i, t) *(t*)((i < (v)->length && i >= 0) ? (&(v)->__buffer[0] + i * (v)->__element_size) : NULL)
#define vector_set(v, i, d) if (i < (v)->length && i >= 0) memcpy((&(v)->__buffer[0] + i * (v)->__element_size), &d, (v)->__element_size)
#define vector_push_back(v, d) __vec_insert(&v, (v)->length, (void*)&d)
#define vector_push_front(v, d) __vec_insert(&v, 0, (void*)&d)
#define vector_pop_back(v) __vec_remove(v, (v)->length - 1, 1)
#define vector_pop_front(v) __vec_remove(v, 0, 1)
#define vector_clear(v) __vec_remove(v, 0, (v)->length)
#define vector_max_length(v) 4294967295UL / ((v)->__element_size - 1)
#define vector_bytes(v) offsetof(vector, __buffer) + ((v)->__element_size * (v)->__capacity)

typedef struct {
  size_t length;
  size_t __capacity;
  size_t __element_size;
  uint8_t __buffer[];
} vector;

vector* __vec_factory(size_t, size_t);

vector* __vec_resize(vector*, size_t);

uint8_t __vec_insert(vector**, size_t, void*);

uint8_t __vec_remove(vector*, size_t, size_t);


#endif  //C_VECTOR_H