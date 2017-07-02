#include <string.h>

#define MAX_DATA_SIZE 44100 * 2 * 4

struct Stack {
  char pile[MAX_DATA_SIZE];
  int stack_size;
};

void resetStack(struct Stack *stack) {
  stack->stack_size = 0;
}

void pushDataToStack(struct Stack *stack, char *data, int data_size) {
  memcpy(stack->pile + stack->stack_size, data, data_size);
  stack->stack_size += data_size;
}
