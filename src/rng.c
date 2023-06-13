//
// Created by marijn on 4/15/23.
//
#include "rng.h"


/*!< variables */  // mod, mul, inc are set in accordance with GCC
static uint32_t LCG_mod		= 0x80000000;
static uint32_t LCG_mul		= 1103515245;
static uint32_t LCG_inc		= 12345;
static uint32_t LCG_seed	= 0;

/*!< static functions */
static inline void lcg(void) {
	LCG_seed = (LCG_mul * LCG_seed + LCG_inc) % LCG_mod;
}

/*!< init / enable / disable */
void start_RNG(void)			{ LCG_seed = UID->ID0 ^ UID->ID1 ^ UID->ID2; }
void set_RNG(uint32_t seed)		{ LCG_seed = seed; }

/*!< usage */
uint32_t RNG_generate(uint32_t max) {
	lcg();  // progress the random algorithm
	return LCG_seed % ((uint64_t)(max) + 1);
}