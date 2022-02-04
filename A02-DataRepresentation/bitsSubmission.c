/*
 * Roei Cohen
 * David D'Attile
 */

 #include<stdio.h>

/*
 * bitAnd - x&y
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Instructor solution uses 4 ops
 */
int bitAnd(int x, int y) {
	//De Morgan's Law: not (x and y) = (not x) or (not y)
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
	// NAND gate is ~(x & y)
	int nand = ~(x&y);
  	return ~(~(x&nand) & ~(nand&y));
}

/*
 * isNotEqual - return 0 if x == y and 1 otherwise
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Instructor solution uses 3 ops
 */
int isNotEqual(int x, int y) {
 //if all bits are the same: 0000
  return !!(x ^ y);
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
  return (x << 31) >> 31;
}

/*
 * ternary - same as x ? y : z
 *   Example: ternary(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Instructor solution uses 7 ops
 */
int ternary(int x, int y, int z) {
  // !x to get if its 0 or not. Create mask based off that result (all 0s or all 1s)
  int x_mask = copyLSB(!x);
  return (~x_mask & y) | (x_mask & z);
}

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Instructor solution uses 6 ops
 */
int bang(int x) {
  // 0 is the only twos complement # that returns the same number
  // in other cases, the sign bit will differ, so we want to check if
  // the two sign bits are the same or not for x and its twos complement

  return ((x ^ (~x + 1)) >> 31) + 1;
}

/*
 * TMax - return maximum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Instructor solution uses 3 ops
 */
int tmax(void) {
  //get 100000000... and flip the bits to get 0111111
  return ~(1 << 31);
}

/*
 * isNonNegative - return 1 if x >= 0, return 0 otherwise
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Instructor solution uses 2 ops
 */
int isNonNegative(int x) {
  //int a = !(x & (1 << 31));
  //shift sign bit to the right: either 11111111... or 000000...
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
  // if x >0 and y> 0 and sum < 0 then overflow 
  // if x <0 and y< 0 and sum > 0 then overflow
  
  //implicity, if x and y both have different leading signs than the sum,
  // then they have the same leading sign. If they're different, just return true.
  // if the sign bit of the result is different for both, then its 1, so 0

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

  // A (positive) power of 2 has exactly one 1 bit, s.t. subtracting 1 will result
  // in all 1's in the units following.
  // So, x & (x - 1) if x is pwr of 2 is 0; nonzero otherwise

  // !!x special case: if x = 0 returns 0
  // x + ~1 + 1 is equiv. to x - 1
  return !!(x & (-1 ^ (1 << 31))) & !!x & !(x & (x + ~1 + 1));
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
 *   Max ops: 30i
 *   Instructor solution uses 13 ops
 */
int float_f2i(unsigned uf) {
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

	printf("***Testing bitAnd***\n");
	printf("0,0 Expecting 0: %d\n", bitAnd(0, 0));
	printf("5,5 Expecting 5: %d\n", bitAnd(5, 5));
	printf("6,5 Expecting 4: %d\n", bitAnd(6, 5));
	printf("-3,5 Expecting 5: %d\n", bitAnd(-3, 5));
	printf("45,19 Expecting 1: %d\n", bitAnd(45, 19));

	printf("***Testing bitXor***\n");
	printf("0,0 Expecting 0: %d\n", bitXor(0, 0));
	printf("-1,-1 Expecting 0: %d\n", bitXor(-1, -1));
	printf("4,5 Expecting 1: %d\n", bitXor(4, 5));
	printf("-4,5 Expecting -7: %d\n", bitXor(-4, 5));
	printf("12,18 Expecting 30: %d\n", bitXor(12, 18));

	printf("***Testing isNotEqual***\n");
	printf("0,0 Expecting 0: %d\n", isNotEqual(0, 0));
	printf("5,5 Expecting 0: %d\n", isNotEqual(5, 5));
	printf("5,3 Expecting 1: %d\n", isNotEqual(5, 3));
	printf("-5,5 Expecting 1: %d\n", isNotEqual(-5, 5));
	printf("-4,-4 Expecting 0: %d\n", isNotEqual(-4, -4));

	printf("***Testing copyLSB***\n");
	printf("5 Expecting -1: %d\n", copyLSB(5));
	printf("19 Expecting -1: %d\n", copyLSB(19));
	printf("0 Expecting 0: %d\n", copyLSB(0));
	printf("6 Expecting 0: %d\n", copyLSB(6));
	printf("-2 Expecting 0: %d\n", copyLSB(-2));

	printf("***Testing ternary***\n");
	printf("0,4,5 Expecting 5: %d\n", ternary(0,4,5));
	printf("2,4,5 Expecting 4: %d\n", ternary(2,4,5));
	printf("-1,4,5 Expecting 4: %d\n", ternary(-1,4,5));
	printf("1,4,5 Expecting 4: %d\n", ternary(1,4,5));
	printf("15,4,5 Expecting 4: %d\n", ternary(15,4,5));

	printf("***Testing bang***\n");
	printf("0 Expecting 1: %d\n", bang(0));
	printf("2 Expecting 0: %d\n", bang(2));
	printf("-1 Expecting 0: %d\n", bang(-1));
	printf("1 Expecting 0: %d\n", bang(1));
	printf("15 Expecting 0: %d\n", bang(15));

	printf("***Testing tmax***\n");
	printf("Expecting 2147483647: %d\n", tmax());

	printf("***Testing isNonNegative***\n");
	printf("Expecting 1: %d\n", isNonNegative(0));
	printf("Expecting 1: %d\n", isNonNegative(1));
	printf("Expecting 0: %d\n", isNonNegative(-1));
	printf("Expecting 1: %d\n", isNonNegative(5));
	printf("Expecting 0: %d\n", isNonNegative(-5));

	printf("***Testing addOK***\n");
	printf("2000000000,-1000000000 Expecting 1: %d\n", addOK(2000000000, -1000000000));
	printf("1,2 Expecting 1: %d\n", addOK(1, 2));
	printf("0,2000000000  Expecting 1: %d\n", addOK(0, 2000000000));
	printf("2000000000,2000000000 Expecting 0: %d\n", addOK(2000000000, 2000000000));
	printf("-2000000000,-2000000000 Expecting 0: %d\n", addOK(-2000000000, -2000000000));

	printf("***Testing isPower2***\n");
	printf("0 Expecting 0: %d\n", isPower2(0));
	printf("1 Expecting 1: %d\n", isPower2(1));
	printf("5 Expecting 0: %d\n", isPower2(5));
	printf("8 Expecting 1: %d\n", isPower2(8));
	printf("-8 Expecting 0: %d\n", isPower2(-8));
	printf("tmin Expecting 0: %d\n", isPower2(-2147483648));


	printf("***Testing float_neg***\n");
	printf("0 Expecting 2147483648: %u\n", float_neg(0)); // yields 2147483648
    printf("2147483648 Expecting 0: %u\n", float_neg(2147483648)); // yields 0 
    printf("2147483652 Expecting 4: %u\n", float_neg(2147483652)); // yields 4
    printf("2147483647 Expecting 2147483647: %u\n", float_neg(2147483647)); // NaN; yields 2147483647
    printf("2139095040 Expecting 2139095040: %u\n", float_neg(2139095040)); // NaN; yields 2139095040 (011111111000...)

	printf("***Testing float_f2i***\n");
	printf("0x1FC00000 Expecting 0: %d\n", float_f2i(0x1FC00000)); // input 0; yields 0
 	printf("0x41200000 Expecting 10: %d\n", float_f2i(0x41200000)); // input 10; yields 10
 	printf("0x412E6666 Expecting 10: %d\n", float_f2i(0x412E6666)); // input 10.9; yields 10
  	printf("0xC12E6666 Expecting -10: %d\n", float_f2i(0xC12E6666)); // input -10.9; yields -10
  	printf("0x4470B1EB Expecting 962: %d\n", float_f2i(0x4470B1EB)); // input 962.8; yields 962
  	printf("0xC82C3D15 Expecting -176372: %d\n", float_f2i(0xC82C3D15)); // input -176372.34; yields -176372
    printf("Expecting -176372: %d\n", float_f2i(0xC82C3D15));
  return 0;
}
