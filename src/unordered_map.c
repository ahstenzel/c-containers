#include "unordered_map.h"

size_t _umap_node_size(size_t element_size) {
	size_t key_size = sizeof(_umap_key_t);
	size_t size_max = element_size > key_size ? element_size : key_size;
	return (element_size + key_size + (size_max - 1)) & ~(size_max - 1);
}

unordered_map* _umap_factory(size_t element_size, size_t capacity) {
	size_t buffer_size = offsetof(unordered_map, _buffer) + capacity + (_umap_node_size(element_size) * capacity);
	unordered_map* umap = malloc(buffer_size);
	if (!umap) { return NULL; }
	memset(umap, 0, buffer_size);
	umap->_capacity = capacity;
	umap->_element_size = element_size;
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
	unordered_map* _umap = *umap;
	_umap_hash_t h = _umap_hash(key);
	size_t pos = _umap_h1(h) & (_umap->_capacity - 1);

	// Linear probe to find an empty bucket
	while (1) {
		uint8_t* ctrl = _umap_ctrl(_umap, pos);
		// Space is empty if high bit is 1
		if ((*ctrl) & _UMAP_EMPTY) {
			// Save lower 8 bits of hash to the control block
			_umap_hash_t h2 = _umap_h2(h);
			memcpy_s(ctrl, 1, &h2, 1);

			// Save the key to the start of the node block
			size_t dest_size = sizeof(_umap_key_t);
			memcpy_s(_umap_node_key(_umap, pos), dest_size, &key, dest_size);

			// Save the data to the end of the node block, aligned by the larger data type
			dest_size = _umap->_element_size;
			memcpy_s(_umap_node_data(_umap, pos), dest_size, data, dest_size);
			break;
		}
		else {
			pos = (pos + 1) & (_umap->_capacity - 1);
		}
	}
	_umap->_length++;
	_umap->_load_count++;
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

unordered_map_it_t* _umap_it(unordered_map* umap) {
	// Error check
	if (!umap || umap->_length == 0) { return NULL; }

	// Construct iterator
	size_t buffer_size = sizeof(unordered_map_it_t);
	unordered_map_it_t* it = malloc(buffer_size);
	if (!it) { return NULL; }
	memset(it, 0, buffer_size);
	it->_index = SIZE_MAX;
	it->_umap = umap;
	
	// Find first valid entry in map
	_umap_it_next(&it);
	return it;
}

void _umap_it_next(unordered_map_it_t** it) {
	// Error check
	if (!it | !(*it)) { return; }

	// Find the next valid position in the buffer
	unordered_map_it_t* _it = *it;
	uint8_t* ctrl = NULL;
	unordered_map* _umap = _it->_umap;
	do {
		// Increment index
		_it->_index++;

		// Reached the end of the array
		if (_it->_index >= _umap->_capacity) {
			free(_it);
			_it = NULL;
			break;
		}

		// Evaluate control byte
		ctrl = _umap_ctrl(_umap, _it->_index);
		if (!(*ctrl & _UMAP_EMPTY)) {
			// Index contains data
			_it->key = *_umap_node_key(_umap, _it->_index);
			_it->data = _umap_node_data(_umap, _it->_index);
			break;
		}
	} while(1);

	return;
}