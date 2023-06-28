#include "priority_queue.h"

size_t _priority_queue_buffer_size(size_t element_size, size_t capacity) {
	size_t meta_size = sizeof(size_t);
	size_t size_max = element_size > meta_size ? element_size : meta_size;
	return ((sizeof(priority_queue_value_t) * capacity) + (element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

priority_queue* _priority_queue_factory(size_t element_size, size_t capacity) {
	size_t buffer_size = offsetof(priority_queue, _buffer) + _priority_queue_buffer_size(element_size, capacity);
	priority_queue* qu = malloc(buffer_size);
	if (!qu) { return NULL; }
	memset(qu, 0, buffer_size);
	qu->_capacity = capacity;
	qu->_element_size = element_size;
	return qu;
}

priority_queue* _priority_queue_resize(priority_queue* qu, size_t new_capacity) {
	// Create new priority_queue & copy data to it
	priority_queue* new_qu = _priority_queue_factory(qu->_element_size, new_capacity);
	if (!new_qu) { return NULL; }
	size_t dest_size = qu->_element_size * qu->_length;
	memcpy_s(new_qu->_buffer, dest_size, qu->_buffer, dest_size);
	new_qu->_length = qu->_length;
	free(qu);
	return new_qu;
}

bool _priority_queue_insert(priority_queue** qu, priority_queue_value_t value, void* data) {
	// Error check
	if (!qu || !(*qu)) { return false; }

	// Resize container
	if ((*qu)->_length >= (*qu)->_capacity) {
		priority_queue* temp = _priority_queue_resize(*qu, (*qu)->_capacity * 2);
		if (!temp) { return false; }
		(*qu) = temp;
	}

	// Copy element to end
	priority_queue* _qu = *qu;
	void* dest = (void*)(_priority_queue_data_pos(_qu, _qu->_length));
	size_t dest_size = _qu->_element_size;
	memcpy_s(dest, dest_size, data, dest_size);
	_qu->_length++;

	// Sort list
	_priority_queue_sort(_qu);
	return true;
}

bool _priority_queue_remove(priority_queue* qu, size_t count) {
	// Error check
	if (!qu || qu->_length < count) { return false; }

	// Decrement _length
	qu->_length -= count;
	return true;
}

bool _priority_queue_remove_value(priority_queue* qu, priority_queue_value_t value) {
	// Error check
	if (!qu || qu->_length == 0) { return false; }

	// Find value
	size_t it = _priority_queue_find_index(qu, value);
	if (it < qu->_capacity) {
		// Shift value block
		void* dest = (void*)(_priority_queue_value_pos(qu, it));
		void* src = (void*)(_priority_queue_value_pos(qu, it + 1));
		size_t move_size = ((qu->_capacity - it - 1) * sizeof(priority_queue_value_t)) + (qu->_capacity * qu->_element_size);
		memmove_s(dest, move_size, src, move_size);

		// Shift data block
		dest = (void*)(_priority_queue_data_pos(qu, it));
		src = (void*)(_priority_queue_data_pos(qu, it + 1));
		move_size = (qu->_capacity - it - 1) * qu->_element_size;
		memmove_s(dest, move_size, src, move_size);

		qu->_length--;
		return true;
	}
	return false;
}

void _priority_queue_sort(priority_queue* qu) {
	// Insertion sort
	size_t dest_elem_size = qu->_element_size;
	size_t dest_value_size = sizeof(priority_queue_value_t);
	for(size_t i=1; i<qu->_length; ++i) {
		// Copy element to temp buffer
		void* tmp_data = malloc(qu->_element_size);
		memcpy_s(tmp_data, dest_elem_size, _priority_queue_data_pos(qu, i), dest_elem_size);
		priority_queue_value_t tmp_value = _priority_queue_value(qu, i);

		// Shift elements
		size_t j=i;
		for(; j >= 0 && _priority_queue_value(qu, j-1) > tmp_value; --j) {
			memcpy_s(_priority_queue_data_pos(qu, j+1), dest_elem_size, _priority_queue_data_pos(qu, j), dest_elem_size);
			memcpy_s(_priority_queue_value_pos(qu, j+1), dest_value_size, _priority_queue_value_pos(qu, j), dest_value_size);
		}

		// Re-insert temp
		memcpy_s(_priority_queue_data_pos(qu, j+1), dest_elem_size, tmp_data, dest_elem_size);
		memcpy_s(_priority_queue_value_pos(qu, j+1), dest_value_size, &tmp_value, dest_value_size);

		// Free temp buffer
		free(tmp_data);
	}
}

priority_queue_it_t* _priority_queue_it(priority_queue* qu) {
	// Error check
	if (!qu || qu->_length == 0) { return NULL; }

	// Construct iterator
	size_t buffer_size = sizeof(priority_queue_it_t);
	priority_queue_it_t* it = malloc(buffer_size);
	if (!it) { return NULL; }
	memset(it, 0, buffer_size);
	it->_index = qu->_length;
	it->_qu = qu;
	
	// Find first valid entry in map
	_priority_queue_it_next(&it);
	return it;
}

size_t _priority_queue_find_index(priority_queue* qu, priority_queue_value_t value) {
	// Binary search
	size_t hi = qu->_length - 1;
	size_t lo = 0;
	size_t md = 0;
	priority_queue_value_t hi_val = 0;
	priority_queue_value_t lo_val = 0;
	priority_queue_value_t md_val = 0;
	do {
		md = lo + (hi - lo) / 2;
		hi_val = _priority_queue_value(qu, hi);
		lo_val = _priority_queue_value(qu, lo);
		md_val = _priority_queue_value(qu, md);
		if (md_val == value) {
			// Value found
			return md;
		}
		else if (md_val < value) {
			// Midpoint too low, move window up
			lo = md + 1;
		}
		else {
			// Midpoint too high, move window down
			hi = md - 1;
		}
	}
	while (hi != lo);
	return qu->_capacity;
}

void* _priority_queue_find(priority_queue* qu, priority_queue_value_t value) {
	// Error check
	if (!qu || qu->_length == 0) { return NULL; }

	// Binary search
	size_t it = _priority_queue_find_index(qu, value);
	return (it < qu->_capacity) ? _priority_queue_data_pos(qu, it) : NULL;
}

void _priority_queue_it_next(priority_queue_it_t** it) {
	// Error check
	if (!it | !(*it)) { return; }

	// Find the next valid position in the buffer
	priority_queue_it_t* _it = *it;
	priority_queue* _qu = _it->_qu;
	if (_it->_index > 0) {
		// Record next positions data
		_it->_index--;
		_it->data = _priority_queue_data_pos(_qu, _it->_index);
		_it->value = _priority_queue_value(_qu, _it->_index);
	}
	else {
		// End reached, invalidate iterator
		free(_it);
		_it = NULL;
	}

	return;
}