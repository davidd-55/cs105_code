#include <stdio.h>
#include <stdlib.h>

int main() {
    // init int value
    int some_int = 15;
    // address where an integer already exists; 4 bytes
    int *int_pointer = &some_int; // deref with '&'; int --> int ptr
    // allocate space for int; don't set
    int *int_pointer2 = (int *)malloc(sizeof(int));

    printf("Heap  : %p\n", int_pointer2); // prints out pointer addr (heap)
    printf("Stack : %p\n", &int_pointer2); // prints out pointer addr (stack --> heap)
    printf("Value : %d\n", *int_pointer2);
    // printf("%d\n", &int_pointer2); - deref with &, reference with *!
    // printf("%d\n", %int_pointer2); - invalid

    // free mem stored on heap
    free(int_pointer2);

    // get int_pointer2 to point to some_int
    int_pointer2 = int_pointer;
    // int_pointer2 = *int_pointer; - won't work; becomes int pointer : int type mismatch
}