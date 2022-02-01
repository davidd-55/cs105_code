#include <stdio.h>

// int is 4 bytes, char is 1 - so message has 2 ints (8 bytes!)
int main(int argc, char *argv[]) {
    char * message = "CS 105!"; // message is pointer to start of char array ('C')
    
    int * first_pointer = (int *)message; // int pointer to message?
    int first =  *first_pointer; // decode from int pointer to int

    int * second_pointer = (int *)(message+4); // int pointer to message starting at '0'
    int second = *second_pointer; // decode from int pointer to int

    // print result to console
    printf("%d\n%d\n", first, second);

    return 0;
}