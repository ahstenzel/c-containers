#include "queue.h"

size_t __queue_buffer_size(size_t element_size, size_t capacity) {
  size_t meta_size = sizeof(size_t);
  size_t size_max = element_size > meta_size ? element_size : meta_size;
  return ((element_size * capacity) + (size_max - 1)) & ~(size_max - 1);
}

queue* __queue_factory(size_t element_size, size_t capacity) {
  queue* qu = malloc(offsetof(queue, __buffer) + __queue_buffer_size(element_size, capacity));
  if (!qu) { return NULL; }
  qu->length = 0;
  qu->__head = 0;
  qu->__tail = 0;
  qu->__capacity = capacity;
  qu->__element_size = element_size;
  return qu;
}

queue* __queue_resize(queue* qu, size_t new_capacity) {
  // Create new queue & copy data to it
  queue* new_qu = __queue_factory(qu->__element_size, new_capacity);
  if (!new_qu) { return NULL; }
  if (qu->__tail <= qu->__head) {
    // Queue wraps around circular buffer, copy in two parts
    size_t __qu_end = (qu->__capacity - qu->__head);
    memcpy(__queue_pos(new_qu, 0), __queue_pos(qu, qu->__head), __qu_end * qu->__element_size);
    memcpy(__queue_pos(new_qu, __qu_end), __queue_pos(qu, 0), (qu->length - __qu_end) * qu->__element_size);
  }
  else {
    // Queue is contained in circular buffer, one copy will get everything
    memcpy(new_qu->__buffer, __queue_pos(qu, qu->__head), qu->length * qu->__element_size);
  }
  new_qu->__head = 0;
  new_qu->__tail = qu->length;
  new_qu->length = qu->length;
  free(qu);
  return new_qu;
}

uint8_t __queue_insert(queue** qu, void* data) {
  // Error check
  if (!qu || !(*qu)) { return 1; }

  // Resize container
  if ((*qu)->length >= (*qu)->__capacity) {
    queue* temp = __queue_resize(*qu, (*qu)->__capacity * 2);
    if (!temp) { return 1; }
    (*qu) = temp;
  }

  // Append to tail
  void* dest = __queue_pos(*qu, (*qu)->__tail);
  memcpy(dest, data, (*qu)->__element_size);
  (*qu)->__tail = ((*qu)->__tail + 1) % (*qu)->__capacity;
  (*qu)->length++;
  return 0;
}

uint8_t __queue_remove(queue* qu, size_t count) {
  // Error check
  if (!qu) { return 1; }

  // Increment head
  size_t new_head = (qu->__head + count) % qu->__capacity;
  qu->__head = (qu->__head <= qu->__tail && new_head > qu->__tail) ? qu->__tail : new_head;
  qu->length = (count > qu->length) ? 0 : (qu->length - count);
  return 0;
}