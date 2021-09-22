#include <stdio.h>
#include "vector.h"
#include "stack.h"
#include "unordered_map.h"
#include "lot.h"

int main() {
  printf("__Vector__\n");
  vector* myvec = vector_create(int);
  for (int i = 0; i < 10; ++i) {
    vector_push_back(myvec, &i);
  }
  for (size_t i = 0; i < myvec->length; ++i) {
    printf("%zd: %d\n", i, vector_get(myvec, i, int));
  }
  vector_destroy(myvec);

  printf("__Stack__\n");
  stack* mystack = stack_create(int);
  for (int i = 0; i < 10; ++i) {
    stack_push(mystack, &i);
  }
  while (mystack->length > 0) {
    printf("%d\n", stack_head(mystack, int));
    stack_pop(mystack);
  }
  stack_destroy(mystack);

  printf("__Unordered Map__\n");
  unordered_map* mymap = unordered_map_create(int);
  int keys[40];
  for (int i = 0; i < 40; ++i) {
    keys[i] = i + 1000;
    unordered_map_insert(mymap, keys[i], &i);
  }
  /*
  for (int i = 0; i < 40; ++i) {
    void* data = unordered_map_find(mymap, keys[i]);
    if (data) {
      int j = *(int*)data;
      printf("%d: %d\n", (int)keys[i], (int)j);
      unordered_map_delete(mymap, keys[i]);
    }
    else {
      printf("Key %d not found!\n", (int)keys[i]);
    }
  }*/
  for (umap_it_t* it = unordered_map_it(mymap); it; unordered_map_it_next(it)) {
    int j = *(int*)(it->data);
    printf("%d: %d\n", (int)it->key, j);
  }
  unordered_map_destroy(mymap);

  printf("__Lot__\n");
  lot* mylot = lot_create(int);
  uint64_t lkeys[40];
  for (int i = 0; i < 40; ++i) {
    lot_insert(mylot, &lkeys[i], &i);
  }
  /*
  for (int i = 0; i < 40; ++i) {
    void* data = lot_find(mylot, lkeys[i]);
    if (data) {
      int j = *(int*)data;
      printf("%d: %d\n", (int)lkeys[i], (int)j);
      lot_delete(mylot, lkeys[i]);
    }
    else {
      printf("Key %d not found!\n", (int)lkeys[i]);
    }
  }*/
  for (lot_it_t* it = lot_it(mylot); it; lot_it_next(it)) {
    int j = *(int*)(it->data);
    printf("%d: %d\n", (int)it->__index, j);
  }
  lot_destroy(mylot);
  return 0;
}