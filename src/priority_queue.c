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