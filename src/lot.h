/*
 * lot.h
 * Smart array with automatic index assignment.
 */

#ifndef C_LOT_H
#define C_LOT_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __LOT_32  // 32 bit keys
typedef uint32_t __lot_key_t;
typedef uint16_t __lot_index_t;
#else // 64 bit keys
typedef uint64_t __lot_key_t;
typedef uint32_t __lot_index_t;
#endif

#define __LOT_DEFAULT_CAPACITY 32

#define __lot_stack_head(l) (__lot_index_t*)(&(l)->__buffer[0] + (sizeof(__lot_index_t) * (l)->__stack_head))
#define __lot_stack_push(l, n) memcpy(__lot_stack_head(l), &n, sizeof(__lot_index_t)); (l)->__stack_head++
#define __lot_stack_pop(l) (l)->__stack_head--;
#define __lot_node(l, i) (&(l)->__buffer[0] + ((l)->__capacity * sizeof(__lot_index_t)) + (((l)->__element_size * 2) * i))
#define __lot_node_ctrl(l, i) (uint8_t*)__lot_node(l, i)
#define __lot_node_data(l, i) (void*)(__lot_node(l, i) + (l)->__element_size)
#define __lot_node_size(e) (e * 2)

#define lot_create(t) __lot_factory(sizeof(t), __LOT_DEFAULT_CAPACITY)
#define lot_destroy(l) free(l)
#define lot_insert(l, k, d) __lot_insert(&l, (__lot_key_t*)k, (void*)&d)
#define lot_find(l, k) __lot_find(l, k)
#define lot_delete(l, k) __lot_delete(l, k)

typedef struct {
  size_t length;
  size_t __capacity;
  size_t __element_size;
  size_t __stack_head;
  uint8_t __buffer[];
} lot;

lot* __lot_factory(size_t, size_t);

lot* __lot_resize(lot*, size_t);

uint8_t __lot_insert(lot**, __lot_key_t*, void*);

void* __lot_find(lot*, __lot_key_t);

uint8_t __lot_delete(lot*, __lot_key_t);

#endif  // C_LOT_H