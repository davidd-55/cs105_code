#include <stdio.h>

int takes_two_ints(int one, int two) {
    if (two >= one) {
        int sum = 0;
        for (int num = one; num <= two; one++) {
            sum += num;
        }
        return sum;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    int two_int = takes_two_ints(4,10);

    printf("The number is: %d\n", two_int);

    return 0;
}