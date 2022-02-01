#include <stdio.h>

/*
 * PARTNER 1
 * PARTNER 2
 */

/*
 * bitAnd - x&y
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Instructor solution uses 4 ops
 */
int bitAnd(int x, int y) {
  return ~(~x | ~y);
}

/*
 * bitXor - x^y
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Instructor solution uses 7 ops
 */
int bitXor(int x, int y) {
  int gate = ~(x&y);
  return ~(~(x&gate)&~(gate&y));
}

/*
 * isNotEqual - return 0 if x == y and 1 otherwise
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Instructor solution uses 3 ops
 */
// can we combine logical operators and bitwise operators in the same function ?
int isNotEqual(int x, int y) {
  /* TODO: implement this function */
  return !!(x^y);
}

/*
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Instructor solution uses 2 ops
 */
int copyLSB(int x) {
  /* TODO: implement this function */
  return -1;
}

/*
 * ternary - same as x ? y : z
 *   Example: ternary(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Instructor solution uses 7 ops
 */
int ternary(int x, int y, int z) {
  /* TODO: implement this function */
  return -1;
}

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Instructor solution uses 6 ops
 */
int bang(int x) {
  /* TODO: implement this function */
  return -1;
}

/*
 * TMax - return maximum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Instructor solution uses 3 ops
 */
int tmax(void) {
  /* TODO: implement this function */
  return -1;
}

/*
 * isNonNegative - return 1 if x >= 0, return 0 otherwise
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Instructor solution uses 2 ops
 */
int isNonNegative(int x) {
  /* TODO: implement this function */
  return -1;
}

/*
 * addOK - Determine if you can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Instructor solution uses 11 ops
 */
int addOK(int x, int y) {
  /* TODO: implement this function */
  return -1;
}

/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Instructor solution uses 10 ops
 */
int isPower2(int x) {
  /* TODO: implement this function */
  return -1;
}

/*
 * float_neg - Return bit-level equivalent of expression -uf for
 *   floating point argument uf (negative to positive and positive
 *   to negative).
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Instructor solution uses 5 ops
 */
unsigned float_neg(unsigned uf) {
  /* TODO: implement this function */
  return -1;
}

/*
 * float_f2i - Return bit-level equivalent of expression (int) uf
 *   (casting/converting float to int) for floating point argument uf.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Instructor solution uses 13 ops
 */
int float_f2i(unsigned uf) {
  /* TODO: implement this function */
  return -1;
}

int main() {
  int result = isNotEqual(4,5);
  printf("%d", result);
  return 0;
}
