#include "unordered_map.h"

size_t __umap_node_size(size_t element_size) {
  size_t key_size = sizeof(__umap_key_t);
  size_t size_max = element_size > key_size ? element_size : key_size;
  return (element_size + key_size + (size_max - 1)) & ~(size_max - 1);
}

unordered_map* __umap_factory(size_t element_size, size_t capacity) {
  unordered_map* umap = malloc(offsetof(unordered_map, __buffer) + capacity + (__umap_node_size(element_size) * capacity));
  if (!umap) { return NULL; }
  umap->length = 0;
  umap->__capacity = capacity;
  umap->__element_size = element_size;
  umap->__load_count = 0;
  umap->__load_factor = __UMAP_DEFAULT_LOAD;
  memset(__umap_ctrl(umap, 0), __UMAP_EMPTY, capacity);
  return umap;
}

unordered_map* __umap_resize(unordered_map* umap, size_t new_capacity) {
  // Create new map
  unordered_map* new_umap = __umap_factory(umap->__element_size, new_capacity);
  if (!new_umap) { return NULL; }

  // Rehash data
  for (size_t i = 0; i < umap->__capacity; ++i) {
    uint8_t* ctrl = __umap_ctrl(umap, i);
    if (!((*ctrl) & __UMAP_EMPTY)) {
      __umap_key_t* _key = __umap_node_key(umap, i);
      void* _data = __umap_node_data(umap, i);
      __umap_insert(&new_umap, *_key, _data);
    }
  }

  // Copy over attributes
  new_umap->__load_factor = umap->__load_factor;

  // Return new map
  free(umap);
  return new_umap;
}

__umap_hash_t __umap_hash(__umap_key_t key) {
  // Hash using basic FNV-1a implementation
  __umap_hash_t hash = __fnv_offset;
  for (size_t i = 0; i < sizeof(__umap_key_t); ++i) {
    hash ^= ((key >> (i * 8)) & 0xFF);
    hash *= __fnv_prime;
  }
  return hash;
}

uint8_t __umap_insert(unordered_map** umap, __umap_key_t key, void* data) {
  // Error check
  if (!umap || !(*umap)) { return 1; }

  // Resize if needed
  if ((*umap)->__load_count / (float)(*umap)->__capacity >= (*umap)->__load_factor) {
    unordered_map* temp = __umap_resize(*umap, (*umap)->__capacity * 2);
    if (!temp) { return 1; }
    (*umap) = temp;
  }

  // Hash the key
  __umap_hash_t h = __umap_hash(key);
  size_t pos = __umap_h1(h) & ((*umap)->__capacity - 1);

  // Linear probe to find an empty bucket
  while (1) {
    uint8_t* ctrl = __umap_ctrl(*umap, pos);
    // Space is empty if high bit is 1
    if ((*ctrl) & __UMAP_EMPTY) {
      // Save lower 8 bits of hash to the control block
      __umap_hash_t h2 = __umap_h2(h);
      memcpy(ctrl, &h2, 1);

      // Save the key to the start of the node block
      memcpy(__umap_node_key(*umap, pos), &key, sizeof(key));

      // Save the data to the end of the node block, aligned by the larger data type
      memcpy(__umap_node_data(*umap, pos), data, (*umap)->__element_size);
      break;
    }
    else {
      pos = (pos + 1) & ((*umap)->__capacity - 1);
    }
  }
  (*umap)->length++;
  (*umap)->__load_count++;
  return 0;
}

uint8_t __umap_delete(unordered_map* umap, __umap_key_t key) {
  // Error check
  if (!umap) { return 1; }

  // Hash key again
  __umap_hash_t h = __umap_hash(key);
  size_t pos = __umap_h1(h) & (umap->__capacity - 1);

  // Linear probe to find key
  while (1) {
    uint8_t* ctrl = __umap_ctrl(umap, pos);
    // Check if this control byte matches lower byte of hash
    __umap_hash_t h2 = __umap_h2(h);
    if (*ctrl == h2) {
      // Verify key at this pos matches
      if (key == *(__umap_node_key(umap, pos))) {
        memset(ctrl, __UMAP_DELETED, 1);
        umap->length--;
        return 0;
      }
    }
    else if (*ctrl == __UMAP_EMPTY) {
      // Empty slot marks the end of the bucket chain
      return 0;
    }
    else {
      // Look at next control byte
      pos = (pos + 1) & (umap->__capacity - 1);
    }
  }
}

void* __umap_find(unordered_map* umap, __umap_key_t key) {
  // Error check
  if (!umap) { return NULL; }

  // Hash key again
  __umap_hash_t h = __umap_hash(key);
  size_t pos = __umap_h1(h) & (umap->__capacity - 1);

  // Linear probe to find key
  while (1) {
    uint8_t* ctrl = __umap_ctrl(umap, pos);
    // Check if this control byte matches lower byte of hash
    __umap_hash_t h2 = __umap_h2(h);
    if (*ctrl == h2) {
      // Verify key at this pos matches
      if (key == *(__umap_node_key(umap, pos))) {
        return __umap_node_data(umap, pos);
      }
    }
    else if (*ctrl == __UMAP_EMPTY) {
      // Empty slot marks the end of the bucket chain
      return NULL;
    }
    else {
      // Look at next control byte
      pos = (pos + 1) & (umap->__capacity - 1);
    }
  }
}