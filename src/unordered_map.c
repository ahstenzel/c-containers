#include "unordered_map.h"

size_t _umap_node_size(size_t element_size) {
  size_t key_size = sizeof(_umap_key_t);
  size_t size_max = element_size > key_size ? element_size : key_size;
  return (element_size + key_size + (size_max - 1)) & ~(size_max - 1);
}

unordered_map* _umap_factory(size_t element_size, size_t capacity) {
  unordered_map* umap = malloc(offsetof(unordered_map, _buffer) + capacity + (_umap_node_size(element_size) * capacity));
  if (!umap) { return NULL; }
  umap->_length = 0;
  umap->_capacity = capacity;
  umap->_element_size = element_size;
  umap->_load_count = 0;
  memset(_umap_ctrl(umap, 0), _UMAP_EMPTY, capacity);
  return umap;
}

unordered_map* _umap_resize(unordered_map* umap, size_t new_capacity) {
  // Create new map
  unordered_map* new_umap = _umap_factory(umap->_element_size, new_capacity);
  if (!new_umap) { return NULL; }

  // Rehash data
  for (size_t i = 0; i < umap->_capacity; ++i) {
    uint8_t* ctrl = _umap_ctrl(umap, i);
    if (!((*ctrl) & _UMAP_EMPTY)) {
      _umap_key_t* _key = _umap_node_key(umap, i);
      void* _data = _umap_node_data(umap, i);
      _umap_insert(&new_umap, *_key, _data);
    }
  }

  // Return new map
  free(umap);
  return new_umap;
}

_umap_hash_t _umap_hash(_umap_key_t key) {
  // Hash using basic FNV-1a implementation
  _umap_hash_t hash = _fnv_offset;
  for (size_t i = 0; i < sizeof(_umap_key_t); ++i) {
    hash ^= ((key >> (i * 8)) & 0xFF);
    hash *= _fnv_prime;
  }
  return hash;
}

bool _umap_insert(unordered_map** umap, _umap_key_t key, void* data) {
  // Error check
  if (!umap || !(*umap)) { return false; }

  // Resize if needed
  if ((*umap)->_load_count / (float)(*umap)->_capacity >= _UMAP_DEFAULT_LOAD) {
    unordered_map* temp = _umap_resize(*umap, (*umap)->_capacity * 2);
    if (!temp) { return false; }
    (*umap) = temp;
  }

  // Hash the key
  _umap_hash_t h = _umap_hash(key);
  size_t pos = _umap_h1(h) & ((*umap)->_capacity - 1);

  // Linear probe to find an empty bucket
  while (1) {
    uint8_t* ctrl = _umap_ctrl(*umap, pos);
    // Space is empty if high bit is 1
    if ((*ctrl) & _UMAP_EMPTY) {
      // Save lower 8 bits of hash to the control block
      _umap_hash_t h2 = _umap_h2(h);
      memcpy(ctrl, &h2, 1);

      // Save the key to the start of the node block
      memcpy(_umap_node_key(*umap, pos), &key, sizeof(key));

      // Save the data to the end of the node block, aligned by the larger data type
      memcpy(_umap_node_data(*umap, pos), data, (*umap)->_element_size);
      break;
    }
    else {
      pos = (pos + 1) & ((*umap)->_capacity - 1);
    }
  }
  (*umap)->_length++;
  (*umap)->_load_count++;
  return true;
}

bool _umap_delete(unordered_map* umap, _umap_key_t key) {
  // Error check
  if (!umap) { return false; }

  // Hash key again
  _umap_hash_t h = _umap_hash(key);
  size_t pos = _umap_h1(h) & (umap->_capacity - 1);

  // Linear probe to find key
  while (1) {
    uint8_t* ctrl = _umap_ctrl(umap, pos);
    // Check if this control byte matches lower byte of hash
    _umap_hash_t h2 = _umap_h2(h);
    if (*ctrl == h2) {
      // Verify key at this pos matches
      if (key == *(_umap_node_key(umap, pos))) {
        memset(ctrl, _UMAP_DELETED, 1);
        umap->_length--;
        return true;
      }
    }
    else if (*ctrl == _UMAP_EMPTY) {
      // Empty slot marks the end of the bucket chain
      return true;
    }
    else {
      // Look at next control byte
      pos = (pos + 1) & (umap->_capacity - 1);
    }
  }
}

void* _umap_find(unordered_map* umap, _umap_key_t key) {
  // Error check
  if (!umap) { return NULL; }

  // Hash key again
  _umap_hash_t h = _umap_hash(key);
  size_t pos = _umap_h1(h) & (umap->_capacity - 1);

  // Linear probe to find key
  while (1) {
    uint8_t* ctrl = _umap_ctrl(umap, pos);
    // Check if this control byte matches lower byte of hash
    _umap_hash_t h2 = _umap_h2(h);
    if (*ctrl == h2) {
      // Verify key at this pos matches
      if (key == *(_umap_node_key(umap, pos))) {
        return _umap_node_data(umap, pos);
      }
    }
    else if (*ctrl == _UMAP_EMPTY) {
      // Empty slot marks the end of the bucket chain
      return NULL;
    }
    else {
      // Look at next control byte
      pos = (pos + 1) & (umap->_capacity - 1);
    }
  }
}

umap_it_t* _umap_it(unordered_map* umap) {
  // Error check
  if (!umap) { return NULL; }

  // Construct iterator
  umap_it_t* it = malloc(sizeof(*it));
  if (!it) { return NULL; }
  it->_index = SIZE_MAX;
  it->_umap = umap;
  
  // Find first valid entry in map
  _umap_next(&it);
  return it;
}

void _umap_next(umap_it_t** it) {
  // Error check
  if (!it | !(*it)) { return; }

  // Find the next valid position in the array
  uint8_t* ctrl = NULL;
  unordered_map* umap = (*it)->_umap;
  do {
    // Increment index
    (*it)->_index++;

    // Reached the end of the array
    if ((*it)->_index >= umap->_capacity) {
      free(*it);
      *it = NULL;
      break;
    }

    // Evaluate control byte
    ctrl = _umap_ctrl(umap, (*it)->_index);
    if (!(*ctrl & _UMAP_EMPTY)) {
      // Index contains data
      (*it)->key = *_umap_node_key(umap, (*it)->_index);
      (*it)->data = _umap_node_data(umap, (*it)->_index);
      break;
    }
  } while(1);

  return;
}