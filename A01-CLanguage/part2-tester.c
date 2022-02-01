#include <stdio.h>
#include <stdlib.h>

#define MAX_INTS 2

int main() {
  unsigned int source[MAX_INTS];
  int j;

  for (j = 0; j < MAX_INTS; j++)
    if (scanf("%u", source + j) == EOF) {
      printf("Too few values!\n");
      exit(1);
    }

  printf("%s\n", (char *)source);

  return 0;
}
