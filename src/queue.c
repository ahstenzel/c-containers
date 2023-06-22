#include "queue.h"

size_t _queue_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

queue* _queue_factory(size_t element_size, size_t capacity) {
  queue* qu = malloc(offsetof(queue, _buffer) + _queue_buffer_size(element_size, capacity));
  if (!qu) { return NULL; }
  qu->_length = 0;
  qu->_head = 0;
  qu->_tail = 0;
  qu->_capacity = capacity;
  qu->_element_size = element_size;
  return qu;
}

queue* _queue_resize(queue* qu, size_t new_capacity) {
  // Create new queue & copy data to it
  queue* new_qu = _queue_factory(qu->_element_size, new_capacity);
  if (!new_qu) { return NULL; }
  if (qu->_tail <= qu->_head) {
    // Queue wraps around circular buffer, copy in two parts
    size_t _qu_end = (qu->_capacity - qu->_head);
    memcpy(_queue_pos(new_qu, 0), _queue_pos(qu, qu->_head), _qu_end * qu->_element_size);
    memcpy(_queue_pos(new_qu, _qu_end), _queue_pos(qu, 0), (qu->_length - _qu_end) * qu->_element_size);
  }
  else {
    // Queue is contained in circular buffer, one copy will get everything
    memcpy(new_qu->_buffer, _queue_pos(qu, qu->_head), qu->_length * qu->_element_size);
  }
  new_qu->_head = 0;
  new_qu->_tail = qu->_length;
  new_qu->_length = qu->_length;
  free(qu);
  return new_qu;
}

bool _queue_insert(queue** qu, void* data) {
  // Error check
  if (!qu || !(*qu)) { return false; }

  // Resize container
  if ((*qu)->_length >= (*qu)->_capacity) {
    queue* temp = _queue_resize(*qu, (*qu)->_capacity * 2);
    if (!temp) { return false; }
    (*qu) = temp;
  }

  // Append to tail
  void* dest = _queue_pos(*qu, (*qu)->_tail);
  memcpy(dest, data, (*qu)->_element_size);
  (*qu)->_tail = ((*qu)->_tail + 1) % (*qu)->_capacity;
  (*qu)->_length++;
  return true;
}

bool _queue_remove(queue* qu, size_t count) {
  // Error check
  if (!qu || qu->_length < count) { return false; }

  // Increment head
  size_t new_head = (qu->_head + count) % qu->_capacity;
  qu->_head = (qu->_head <= qu->_tail && new_head > qu->_tail) ? qu->_tail : new_head;
  qu->_length -= count;
  return true;
}