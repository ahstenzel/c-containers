#include "dequeue.h"

size_t _dequeue_buffer_size(size_t element_size, size_t capacity) {
	size_t meta_size = sizeof(size_t);
	size_t size_max = element_size > meta_size ? element_size : meta_size;
	return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

dequeue_t* _dequeue_factory(size_t element_size, size_t capacity) {
	size_t buffer_size = offsetof(dequeue_t, _buffer) + _dequeue_buffer_size(element_size, capacity);
	dequeue_t* qu = malloc(buffer_size);
	if (!qu) { return NULL; }
	memset(qu, 0, buffer_size);
	qu->_capacity = capacity;
	qu->_element_size = element_size;
	return qu;
}

dequeue_t* _dequeue_resize(dequeue_t* qu, size_t new_capacity) {
	// Create new dequeue & copy data to it
	dequeue_t* new_qu = _dequeue_factory(qu->_element_size, new_capacity);
	if (!new_qu) { return NULL; }
	if (qu->_tail <= qu->_head) {
		// Queue wraps around circular buffer, copy in two parts
		size_t _qu_end = (qu->_capacity - qu->_head);
		size_t dest_size_start = _qu_end * qu->_element_size;
		size_t dest_size_end = (qu->_length - _qu_end) * qu->_element_size;
		memcpy_s(_dequeue_pos(new_qu, 0), dest_size_start, _dequeue_pos(qu, qu->_head), dest_size_start);
		memcpy_s(_dequeue_pos(new_qu, _qu_end), dest_size_end, _dequeue_pos(qu, 0), dest_size_end);
	}
	else {
		// Queue is contained in circular buffer, one copy will get everything
		size_t dest_size = qu->_length * qu->_element_size;
		memcpy_s(new_qu->_buffer, dest_size, _dequeue_pos(qu, qu->_head), dest_size);
	}
	new_qu->_head = 0;
	new_qu->_tail = qu->_length;
	new_qu->_length = qu->_length;
	free(qu);
	return new_qu;
}

void* _dequeue_insert_front(dequeue_t** qu, void* data) {
	// Error check
	if (!qu || !(*qu)) { return NULL; }

	// Resize container
	if ((*qu)->_length >= (*qu)->_capacity) {
		dequeue_t* temp = _dequeue_resize(*qu, (*qu)->_capacity * 2);
		if (!temp) { return NULL; }
		(*qu) = temp;
	}

	// Append to tail
	dequeue_t* _qu = *qu;
	void* dest = _dequeue_pos(_qu, _qu->_tail);
	size_t dest_size = _qu->_element_size;
	memcpy_s(dest, dest_size, data, dest_size);
	_qu->_tail = (_qu->_tail + 1) % _qu->_capacity;
	_qu->_length++;
	return dest;
}

void* _dequeue_insert_back(dequeue_t** qu, void* data) {
	// Error check
	if (!qu || !(*qu)) { return NULL; }

	// Resize container
	if ((*qu)->_length >= (*qu)->_capacity) {
		dequeue_t* temp = _dequeue_resize(*qu, (*qu)->_capacity * 2);
		if (!temp) { return NULL; }
		(*qu) = temp;
	}

	// Append to head
	dequeue_t* _qu = *qu;
	_qu->_length++;
	_qu->_head = (_qu->_head == 0) ? _qu->_capacity : _qu->_head - 1;
	void* dest = _dequeue_pos(_qu, _qu->_head);
	size_t dest_size = _qu->_element_size;
	memcpy_s(dest, dest_size, data, dest_size);
	return dest;
}

void _dequeue_remove_front(dequeue_t* qu, size_t count) {
	// Error check
	if (!qu || qu->_length < count) { return; }

	// Decrement tail
	size_t new_tail = (qu->_tail < count) ? (qu->_capacity - count + qu->_tail) : qu->_tail - count;
	qu->_tail = (qu->_head <= qu->_tail && new_tail < qu->_head) ? qu->_head : new_tail;
	qu->_length -= count;
	return;
}

void _dequeue_remove_back(dequeue_t* qu, size_t count) {
	// Error check
	if (!qu || qu->_length < count) { return; }

	// Increment head
	size_t new_head = (qu->_head + count) % qu->_capacity;
	qu->_head = (qu->_head <= qu->_tail && new_head > qu->_tail) ? qu->_tail : new_head;
	qu->_length -= count;
	return;
}