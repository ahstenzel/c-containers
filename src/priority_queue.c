#include "priority_queue.h"

size_t _priority_queue_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((sizeof(priority_queue_value_t) * capacity) + (element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

priority_queue* _priority_queue_factory(size_t element_size, size_t capacity) {
  priority_queue* qu = malloc(offsetof(priority_queue, _buffer) + _priority_queue_buffer_size(element_size, capacity));
  if (!qu) { return NULL; }
  qu->_length = 0;
  qu->_capacity = capacity;
  qu->_element_size = element_size;
  return qu;
}

priority_queue* _priority_queue_resize(priority_queue* qu, size_t new_capacity) {
  // Create new priority_queue & copy data to it
  priority_queue* new_qu = _priority_queue_factory(qu->_element_size, new_capacity);
  if (!new_qu) { return NULL; }
  memcpy(new_qu->_buffer, qu->_buffer, qu->_element_size * qu->_length);
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
  void* dest = (void*)(_priority_queue_data_pos(*qu, (*qu)->_length));
  memcpy(dest, data, (*qu)->_element_size);
  (*qu)->_length++;

  // Sort list
  _priority_queue_sort(*qu);
  return true;
}

bool _priority_queue_remove(priority_queue* qu, size_t count) {
  // Error check
  if (!qu) { return false; }

  // Decrement _length
  qu->_length -= count;
  return true;
}

void _priority_queue_sort(priority_queue* qu) {
  // Insertion sort
  for(size_t i=1; i<qu->_length; ++i) {
    // Copy element to temp buffer
    void* tmp_data = malloc(qu->_element_size);
    memcpy_s(tmp_data, qu->_element_size, _priority_queue_data_pos(qu, i), qu->_element_size);
    priority_queue_value_t tmp_value = _priority_queue_value(qu, i);

    // Shift elements
    size_t j=i;
    for(; j >= 0 && _priority_queue_value(qu, j-1) > tmp_value; --j) {
      memcpy_s(_priority_queue_data_pos(qu, j+1), qu->_element_size, _priority_queue_data_pos(qu, j), qu->_element_size);
      memcpy_s(_priority_queue_value_pos(qu, j+1), sizeof(priority_queue_value_t), _priority_queue_value_pos(qu, j), sizeof(priority_queue_value_t));
    }

    // Re-insert temp
    memcpy_s(_priority_queue_data_pos(qu, j+1), qu->_element_size, tmp_data, qu->_element_size);
    memcpy_s(_priority_queue_value_pos(qu, j+1), sizeof(priority_queue_value_t), &tmp_value, sizeof(priority_queue_value_t));

    // Free temp buffer
    free(tmp_data);
  }
}