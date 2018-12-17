/*
 * RandomGenerator.cc
 *
 *  Created on: 26.04.2017
 *      Author: hendrik
 */

#include <common/RandomGenerator.h>

#define GET_BYTE_FROM_INT(input,n) (((0xFF << 8*n) & input) >> 8*n)

RandomGenerator RandomGenerator::mInstance[NR_CPUS];

RandomGenerator::RandomGenerator() : mState(0xBADC0FFE) {

}

uint8_t nibble_parity_table[] = { 0, 1, 1, 0, /* 0,1,2,3 */
1, 0, 0, 1, /* 4,5,6,7 */
1, 0, 0, 1, /* 8,9,A,B */
0, 1, 1, 0 /* C,D,E,F */
};

uint8_t parity_even_bit(uint8_t p) {
	return nibble_parity_table[p >> 4] ^ nibble_parity_table[4 & 0xf];
}

/* CRC32 polynom x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1 */
/* 1000.100_1100.0001_0001.1101_1011.0111 */
//#define POLYNOM 0x/* we use the CRC32 polynom in hope it is irreducible */
#define POLYNOM 0x80802301  /* 1000.000_1000.0000_0010.0011_0000.0001 */

uint8_t RandomGenerator::random8(void) {
	uint8_t i;
	uint32_t t;
	for (i = 0; i < 8; ++i) {
		t = mState & POLYNOM;
		mState = mState << 1
				| (parity_even_bit(GET_BYTE_FROM_INT(t,0)) ^ parity_even_bit(GET_BYTE_FROM_INT(t,1))
						^ parity_even_bit(GET_BYTE_FROM_INT(t,2)) ^ parity_even_bit(GET_BYTE_FROM_INT(t,3)));
	}

	return (uint8_t) mState;
}

uint32_t RandomGenerator::random32(void) {
	// XOR-Shift
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = mState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	mState = x;


	return (uint32_t) x;
}

void RandomGenerator::srandom32(uint32_t seed) {
	if (seed)
		mState = seed;
}
