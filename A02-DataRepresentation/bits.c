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
  // For float_neg we isolated the exp representation bits, checked if they were all 1's
  // (and returned uf if so), then performed a sign flip with uf xor tmin. More detail can be
  // found in the inline code comments.

  // bit shift exp byte (bits 30,...,23) right; mask with 0xFF (255; ...0011111111)
  // so that we are left with usigned value of exp
  unsigned uf_exp = (uf >> 23) & 0xFF;
  
  // check if uf_exp equals 0xFF; if so NaN
  // this is a special case; by def a binary representation of a float
  // is NaN if all exp bits = 0xFF (basically, if they're all 1's)
  if (uf_exp == 0xFF) {
    return uf;
  }

  // return sign flip by uf xor 100...0
  // this preserves all other values except the leading sign bit, which it flips
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
  // For float_f2i, we first isolated the sign, exp, and frac bits. Then, we 
  // checked for NaN (exp all 1's) edge case, the overflow (exp - bias >= 31) edge case, and the
  // |uf| < 1 (exp < bias) edge case and returned appropriate values for each. Finally, we bit shifted
  // an integer containing only the frac bits left or right depending on the value of E, preserved the sign,
  // and returned the resulting value. More detail can be found in the inline code comments.

  // isolate sign bit, exp bits, and frac bits with masking
  // declare bias as 127
  unsigned sign = (uf >> 31) & 0x1;
  unsigned exp = (uf >> 23) & 0xFF; 
  unsigned frac = uf & 0x7FFFFF;
  unsigned bias = 0x7F;
  int final = frac;

  // edge case NaN or infinity; return 0x80000000u
  if (exp == 0xFF) {
    return 0x80000000u;
  }

  // edge case if exp < bias; return 0 since float < 1 
  if (exp < bias) {
    return 0x0;
  }

  // calc E by subtracting bias from exp_bits
  // unsigned okay bc guaranteed that exp >= bias by above if clause
  unsigned e = exp - bias;

  // edge case where large E results in overflow; return 0x80000000u
  // checking for geq 31 because the MSB should be reserved for sign
  if (e >= 31) {
    return 0x80000000u;
  }

  // e is very positive; shift left for larger number
  if (e > 22) {
    final = frac << (e - 23);
  } else { 
    final = frac >> (23 - e);
  }

  // add extra 1 bit to left side of integer 
  // doing this because fractional represents decimal, not 1 + decimal
  final = final + (1 << e);

  // if sign bit is 1 (negative), negate return int
  if (sign) {
    final = -final;
  }

  return final;
}

int main() {
  // printf("%d\n", bang(-0));
  // printf("%d\n", bang(0));
  // printf("%d\n", bang(1));
  // printf("%d\n", bang(-5));
  // printf("%d\n", bang(43));

  // printf("%u\n", float_neg(0)); // yields 2147483648
  // printf("%u\n", float_neg(2147483648)); // yields 0 
  // printf("%u\n", float_neg(2147483652)); // yields 4
  // printf("%u\n", float_neg(2147483647)); // NaN; yields 2147483647
  // printf("%u\n", float_neg(2139095040)); // NaN; yields 2139095040 (011111111000...)

  printf("%d\n", float_f2i(0x1FC00000)); // input 0; yields 0
  printf("%d\n", float_f2i(0x41200000)); // input 10; yields 10
  printf("%d\n", float_f2i(0x412E6666)); // input 10.9; yields 10
  printf("%d\n", float_f2i(0xC12E6666)); // input -10.9; yields -10
  printf("%d\n", float_f2i(0x4470B1EB)); // input 962.8; yields 962
  printf("%d\n", float_f2i(0xC82C3D15)); // input -176372.34; yields -176372

  return 0;
}
