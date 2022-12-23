/*
 * vector.h
 * Dynamically resizing array.
 */

#ifndef C_VECTOR_H
#define C_VECTOR_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define __VECTOR_DEFAULT_CAPACITY 1

#define __vec_pos(v, i) &(v)->__buffer[0] + ((i) * (v)->__element_size)

#define vector_create(t) __vec_factory(sizeof(t), __VECTOR_DEFAULT_CAPACITY)
#define vector_destroy(v) free(v)
#define vector_get(v, i, t) *(t*)((i < (v)->__length && i >= 0) ? __vec_pos(v, i) : NULL)
#define vector_set(v, i, d) { if (i < (v)->__length && i >= 0) memcpy(__vec_pos(v, i), &d, (v)->__element_size) }
#define vector_size(v) (v)->__length
#define vector_push_back(v, d) __vec_insert(&v, (v)->__length, (void*)d)
#define vector_push_front(v, d) __vec_insert(&v, 0, (void*)d)
#define vector_pop_back(v) __vec_remove(v, (v)->__length - 1, 1)
#define vector_pop_front(v) __vec_remove(v, 0, 1)
#define vector_clear(v) __vec_remove(v, 0, (v)->__length)
#define vector_bytes(v) (v) ? (offsetof(vector, __buffer) + __vec_buffer_size((v)->__element_size, (v)->__capacity)) : 0

typedef struct {
  size_t __length;
  size_t __capacity;
  size_t __element_size;
  uint8_t __buffer[];
} vector;

size_t __vec_buffer_size(size_t, size_t);

vector* __vec_factory(size_t, size_t);

vector* __vec_resize(vector*, size_t);

uint8_t __vec_insert(vector**, size_t, void*);

uint8_t __vec_remove(vector*, size_t, size_t);


#endif  //C_VECTOR_H