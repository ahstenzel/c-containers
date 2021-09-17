#include <stdio.h>
#include "cvector.h"
#include "cstack.h"

int main() {
  printf("__Vector__\n");
  vector* myvec = vector_create(int);
  for (int i = 0; i < 10; ++i) {
    vector_push_back(myvec, i);
  }
  for (size_t i = 0; i < myvec->length; ++i) {
    printf("%d: %d\n", i, vector_get(myvec, i, int));
  }
  vector_destroy(myvec);

  printf("__Stack__\n");
  stack* mystack = stack_create(int);
  for (int i = 0; i < 10; ++i) {
    stack_push(mystack, i);
  }
  while (mystack->length > 0) {
    printf("%d\n", stack_head(mystack, int));
    stack_pop(mystack);
  }
  stack_destroy(mystack);
  return 0;
}