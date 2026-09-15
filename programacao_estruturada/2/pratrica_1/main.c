#include "arithmetic.h"
#include <stdio.h>

int main(void) {
  const int x = 10;
  const int y = 5;

  printf("Addition: %d + %d = %d\n", x, y, add(x, y));
  printf("Subtraction: %d - %d = %d\n", x, y, subtract(x, y));

  return 0;
}