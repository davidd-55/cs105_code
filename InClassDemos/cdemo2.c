#include <stdlib.h>
#include <stdio.h>

void print_ints(int *array, int size) {
    // print newline
    printf("\n");
    for (int i = 0; i < size; i++) {
        printf("%d\n", array[i]);
    }
}

int main() {
    // stored on heap
    // using pointer syntax, compiler does not know size
    int *array1 = (int *)malloc(sizeof(int) * 14);
    int *array2 = (int *)malloc(sizeof(int) * 3);

    // stored on stack; STILL POINTERS!
    // using array stack, C compiler knows size of array
    int array3[] = {1,2,3,4};
    int array4[4] = {5,6,7,8};

    print_ints(array1, 14);
    print_ints(array2, 3);
    print_ints(array3, 4);
    // sizeof(array) give number of bytes allocated; not number of items/indices
    print_ints(array4, sizeof(array4) / sizeof(array4[0]));

    // print off just 7 and 8 from array4?
    print_ints(array4 + 2, 2);
}