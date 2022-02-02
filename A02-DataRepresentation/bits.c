#include <stdio.h>

/*
 * Roei Cohen
 * David D'Attile
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
  return ~( ~(x & gate) & ~(gate & y) );
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
  // left shift LSB, right shift to copy it
  return x << 31 >> 31;
}

/*
 * ternary - same as x ? y : z
 *   Example: ternary(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Instructor solution uses 7 ops
 */
int ternary(int x, int y, int z) {
  return (~copyLSB(!x) & y) ^ (copyLSB(!x) & z);
}

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Instructor solution uses 6 ops
 */
int bang(int x) {
  // 0 has unique property of 2's comp. also being zero 
  // 0 xor (~0 + 1) = 0; shight right 31; add 1 equals 1
  // anything else x: x xor (~x + 1) = 1...; shift right 31 yields 1...1; add 1 equals 0
  return ((x ^ (~x + 1)) >> 31) + 1;
}

/*
 * TMax - return maximum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Instructor solution uses 3 ops
 */
int tmax(void) {
  return ~(1 << 31) ;
}

/*
 * isNonNegative - return 1 if x >= 0, return 0 otherwise
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Instructor solution uses 2 ops
 */
int isNonNegative(int x) {
  return !(x >> 31);
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
  // bit shift MSB from left to right
  int xbit = x >> 31;
  int ybit = y >> 31;
  int addbit = (x + y) >> 31;

  // x and y have diff signs: always 1
  // x and y have same sign: 1 iff addbit has same sign
  return !((xbit ^ addbit) & (ybit ^ addbit));
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
  // power 2 is only 1 bit; no leading bit is 1
  // !!x: if x = 0 returns 0
  // x + ~1 + 1 is equiv. to x - 1
  // x & (x - 1) if x is pwr of 2 is 0; nonzero otherwise
  return !!x & !(x & (x + ~1 + 1));
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
  // bit shift exp byte right; mask with 0xFF (255; ...0011111111)
  unsigned uf_exp = (uf >> 22) & 0xFF;
  
  // check if uf_exp equals 0xFF; if so NaN
  if (uf_exp == 0xFF) {
    return uf;
  }

  // return sign flip by uf xor 100...0
  return uf ^ (1 << 31);
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
  // isolate important vals with masking
  unsigned sign_bit = (uf >> 31) & 0x1;
  unsigned exp_bits = (1 >> 22) & 0xFF;
  unsigned frac_bits = uf & 0x7FFFFF;

  
  
  return -1;
}

int main() {
  // printf("%d\n", bang(-0));
  // printf("%d\n", bang(0));
  // printf("%d\n", bang(1));
  // printf("%d\n", bang(-5));
  // printf("%d\n", bang(43));

  printf("%u\n", float_neg(0)); // yields 2147483648
  printf("%u\n", float_neg(2147483648)); // yields 0
  printf("%u\n", float_neg(2147483652)); // yields 4
  printf("%u\n", float_neg(2147483647)); // NaN; yields 2147483647
  printf("%u\n", float_neg(1069547520)); // NaN; yields 1069547520 (011111111000...)
  

  return 0;
}
