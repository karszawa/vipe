#include <string.h>

#define MAX_DATA_SIZE 44100 * 2 * 4

class Stack {
public:

  char pile[MAX_DATA_SIZE];
  int stack_size;

  Stack() : stack_size(0) {

  }

  void reset() {
    this->stack_size = 0;
  }

  void shift(size_t shift_size) {
    memcpy(this->pile, this->pile + shift_size, shift_size);
    memset(this->pile + shift_size, 0, MAX_DATA_SIZE - shift_size);

    this->stack_size -= shift_size;
  }

  void push_data(char *data, size_t data_size) {
    memcpy(this->pile + this->stack_size, data, data_size);
    this->stack_size += data_size;
  }
};
