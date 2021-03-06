#include "lot.h"

lot* __lot_factory(size_t element_size, size_t capacity) {
  // Construct object
  lot* lt = malloc(offsetof(lot, __buffer) + (sizeof(__lot_key_t) * capacity) + (__lot_node_size(element_size) * capacity));
  if (!lt) { return NULL; }
  lt->length = 0;
  lt->__capacity = capacity;
  lt->__element_size = element_size;
  lt->__stack_head = 0;
  // Initialize buffer
  memset(__lot_node(lt, 0), 0, __lot_node_size(element_size) * capacity);
  for (size_t i = 0; i < capacity; ++i) {
    __lot_stack_push(lt, i);
  }
  return lt;
}

lot* __lot_resize(lot* lt, size_t new_capacity) {
  // Create new lot & copy data to it
  lot* new_lt = __lot_factory(lt->__element_size, new_capacity);
  if (!new_lt) { return NULL; }
  memcpy(__lot_node_ctrl(new_lt, 0), __lot_node_ctrl(lt, 0), __lot_node_size(lt->__element_size) * lt->length);
  new_lt->length = lt->length;
  new_lt->__stack_head = lt->__stack_head;
  
  // Push new array entries
  for (size_t i = lt->__capacity; i < new_capacity; ++i) {
    __lot_stack_push(new_lt, i);
  }
  free(lt);
  return new_lt;
}

uint8_t __lot_insert(lot** lt, __lot_key_t* key, void* data) {
  // Error check
  if (!lt || !(*lt)) { return 1; }

  // Resize if needed
  if ((*lt)->length >= (*lt)->__capacity) {
    lot* temp = __lot_resize(*lt, (*lt)->__capacity * 2);
    if (!temp) { return 1; }
    (*lt) = temp;
  }

  // Grab index from the stack
  __lot_index_t index = *__lot_stack_head(*lt);
  __lot_stack_pop(*lt);

  // Increment count at node
  uint8_t* count_ref = __lot_node_ctrl(*lt, index);
  *count_ref += 1;    // Increment value
  *count_ref |= 0x80; // Set high bit

  // Construct key
  if (key) {
    *key = __lot_key(*count_ref, index);
  }

  // Copy data to node
  memcpy(__lot_node_data(*lt, index), data, (*lt)->__element_size);
  (*lt)->length++;
  return 0;
}

void* __lot_find(lot* lt, __lot_key_t key) {
  // Error check
  if (!lt) { return NULL; }

  // Decompose key into count and index
  uint8_t count = __lot_key_count(key);
  __lot_index_t index = __lot_key_index(key);

  // Check if high bit at metadata is set & the counts match
  uint8_t node_count = *__lot_node_ctrl(lt, index);
  if (node_count & 0x80 && (node_count & 0x7F) == count) {
    return __lot_node_data(lt, index);
  }
  
  // Either the index was empty or the keys count didnt match
  return NULL;
}

uint8_t __lot_delete(lot* lt, __lot_key_t key) {
  // Error check
  if (!lt) { return 1; }

  // Decompose key into count and index
  uint8_t count = __lot_key_count(key);
  __lot_index_t index = __lot_key_index(key);

  // Check if high bit at metadata is set & the counts match
  uint8_t* node_count_ref = __lot_node_ctrl(lt, index);
  if (*node_count_ref & 0x80 && (*node_count_ref & 0x7F) == count) {
    // Clear metadata high bit
    *node_count_ref = (*node_count_ref) & 0x7F;

    // Push index back onto the stack
    __lot_stack_push(lt, index);
    lt->length--;
  }

  // Either the index was empty or the keys count didnt match
  return 0;
}

lot_it_t* __lot_it(lot* lt) {
  // Error check
  if (!lt) { return NULL; }

  // Construct iterator
  lot_it_t* it = malloc(sizeof(*it));
  if (!it) { return NULL; }
  it->__index = SIZE_MAX;
  it->__lot = lt;

  // Find first valid entry in map
  __lot_next(&it);
  return it;
}

void __lot_next(lot_it_t** it) {
  // Error check
  if (!it || !(*it)) { return; }

  // Find the next valid position in the array
  uint8_t* ctrl = NULL;
  lot* lt = (*it)->__lot;
  do {
    // Increment index
    (*it)->__index++;

    // Reached the end of the array
    if ((*it)->__index >= lt->__capacity) {
      free(*it);
      *it = NULL;
      break;
    }

    // Evaluate control byte
    ctrl = __lot_node_ctrl(lt, (*it)->__index);
    if (*ctrl & 0x80) {
      // Index contains data
      (*it)->data = __lot_node_data(lt, (*it)->__index);
      break;
    }
  } while (1);

  return;
}