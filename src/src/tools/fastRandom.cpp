#include "fastRandom.h"

fastRandomClass fastRandom;

uint32_t xorshift32()
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    static uint32_t state = fastRandom.rnd64();
	uint32_t x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	state = x;
	return x;
}

uint64_t xorshift64()
{
    static uint64_t state = fastRandom.rnd64();
	uint64_t x = state;
	x^= x << 13;
	x^= x >> 7;
	x^= x << 17;
	state = x;
	return x;
}
