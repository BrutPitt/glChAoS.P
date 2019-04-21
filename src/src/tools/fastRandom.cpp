////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license
//  
////////////////////////////////////////////////////////////////////////////////
//
//  From George Marsaglia Algorithms <geo@stat.fsu.edu> 
//
////////////////////////////////////////////////////////////////////////////////

#include "fastRandom.h"

fastRandomClass fastRandom;

uint32_t xorshift32()
{
	// Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" 
    static uint32_t state = fastRandom.rnd32();
	uint32_t x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	state = x;
	return x;
}

uint64_t xorshift64()
{
    // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" 
    static size_t state = fastRandom.rnd64();
	uint64_t x = state;
	x^= x << 13;
	x^= x >> 7;
	x^= x << 17;
	state = x;
	return x;
}
