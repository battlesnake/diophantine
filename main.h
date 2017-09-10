#pragma once
#include <stdint.h>
#include <stdbool.h>

void progress_zero();
void progress_update();

bool solution_verify(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void solution_found(uint32_t a, uint32_t b, uint32_t c, uint32_t d, bool sur);

extern const uint32_t max;

static inline uint32_t calc_dmax(uint32_t abcmax)
{
	return (85 * abcmax) / 64 + 1;
}

static inline uint32_t calc_dmin(uint32_t abcmax)
{
	return abcmax + 4;
}

static inline __uint128_t pow4b(uint64_t x)
{
	uint64_t y = (uint64_t) x * x;
	return (__uint128_t) y * y;
}

static inline uint32_t pow4i(uint32_t x)
{
	uint32_t y = x * x;
	return y * y;
}

