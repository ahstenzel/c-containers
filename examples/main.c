#include <stdio.h>
#include "vector.h"
#include "stack.h"
#include "unordered_map.h"
#include "queue.h"
#include "priority_queue.h"
#include "dequeue.h"

int main() {
  printf("__Vector__\n");
  vector_t* myvec = vector_create(int);
  for (int i = 0; i < 10; ++i) {
    vector_push_back(myvec, &i);
  }
  for (size_t i = 0; i < vector_size(myvec); ++i) {
    printf("%zd: %d\n", i, *(int*)vector_get(myvec, i));
  }
  vector_destroy(myvec);

  printf("__Stack__\n");
  stack_t* mystack = stack_create(int);
  for (int i = 0; i < 10; ++i) {
    stack_push(mystack, &i);
  }
  while (stack_size(mystack) > 0) {
    printf("%d\n", *(int*)stack_head(mystack));
    stack_pop(mystack);
  }
  stack_destroy(mystack);

  printf("__Unordered Map__\n");
  unordered_map_t* mymap = unordered_map_create(int);
  int keys[40];
  for (int i = 0; i < 40; ++i) {
    keys[i] = i + 1000;
    unordered_map_insert(mymap, keys[i], &i);
  }
  for (unordered_map_it_t* it = unordered_map_it(mymap); it; unordered_map_it_next(it)) {
    int j = *(int*)(it->data);
    printf("%d: %d\n", (int)it->key, j);
  }
  unordered_map_destroy(mymap);

  printf("__Queue__\n");
  queue_t* myqueue = queue_create(int);
  for (int i = 0; i < 15; ++i) {
    queue_push(myqueue, &i);
  }
  for (int i = 0; i < 15; ++i) {
    queue_pop(myqueue);
  }
  for (int i = 0; i < 40; ++i) {
    queue_push(myqueue, &i);
  }
  for (int i = 0; i < 40; ++i) {
    printf("%d\n", *(int*)queue_head(myqueue));
    queue_pop(myqueue);
  }
  queue_destroy(myqueue);
  return 0;
}