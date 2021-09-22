/*
 * cunordered_map.h
 * Hash table of key-value pairs.
 * Implemented as a simplified Swiss Table architecture.
 */

#ifndef C_UMAP_H
#define C_UMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __UMAP_32  // 32 bit hash
typedef uint32_t __umap_key_t;
typedef uint32_t __umap_hash_t;
#define __fnv_offset 2166136261U;
#define __fnv_prime 16777619U;
#else // 64 bit hash
typedef uint64_t __umap_key_t;
typedef uint64_t __umap_hash_t;
#define __fnv_offset 14695981039346656037UL;
#define __fnv_prime 1099511628211UL;
#endif

#define __UMAP_DEFAULT_CAPACITY 32
#define __UMAP_DEFAULT_LOAD 0.875f
#define __UMAP_EMPTY 0x80     // 0b1000 0000
#define __UMAP_DELETED 0xFE   // 0b1111 1110
#define __UMAP_SENTINEL 0xFF  // 0b1111 1111

#define __align_to(x, y) (x + (y - 1)) & ~(y - 1)
#define __umap_h1(h) h >> 7
#define __umap_h2(h) h & 0x7F
#define __umap_ctrl(u, i) (uint8_t*)(&(u)->__buffer[0] + i)
#define __umap_node(u, i) (&(u)->__buffer[0] + (u)->__capacity + (__umap_node_size((u)->__element_size) * i))
#define __umap_node_key(u, i) (__umap_key_t*)__umap_node(u, i)
#define __umap_node_data(u, i) (void*)(__umap_node(u, i) + ((u)->__element_size > sizeof(__umap_key_t) ? (u)->__element_size : sizeof(__umap_key_t)))

#define unordered_map_create(t) __umap_factory(sizeof(t), __UMAP_DEFAULT_CAPACITY)
#define unordered_map_destroy(u) free(u)
#define unordered_map_insert(u, k, d) __umap_insert(&u, k, (void*)d)
#define unordered_map_find(u, k) __umap_find(u, k)
#define unordered_map_delete(u, k) __umap_delete(u, k)
#define unordered_map_set_load(u, f) { if (u) u->__load_factor = f; }
#define unordered_map_clear(u) memset(__umap_ctrl(u, 0), __UMAP_EMPTY, (u)->__capacity)
#define unordered_map_it(u) __umap_it(u)
#define unordered_map_it_next(i) __umap_next(&i)
#define unordered_map_rehash(u) { unordered_map* __umap_temp__ = __umap_resize(u, (u)->__capacity); if (__umap_temp__) u = __umap_temp__; }

typedef struct {
  size_t length;
  size_t __capacity;
  size_t __element_size;
  size_t __load_count;
  float __load_factor;
  uint8_t __buffer[];
} unordered_map;

typedef struct {
  unordered_map* __umap;
  void* data;
  __umap_key_t key;
  size_t __index;
} umap_it_t;

size_t __umap_node_size(size_t);

unordered_map* __umap_factory(size_t, size_t);

unordered_map* __umap_resize(unordered_map*, size_t);

__umap_hash_t __umap_hash(__umap_key_t);

uint8_t __umap_insert(unordered_map**, __umap_key_t, void*);

uint8_t __umap_delete(unordered_map*, __umap_key_t);

void* __umap_find(unordered_map*, __umap_key_t);

umap_it_t* __umap_it(unordered_map*);

void __umap_next(umap_it_t**);

#endif  //C_UMAP_H