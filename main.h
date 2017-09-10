#pragma once
#include <stdint.h>
#include <stdbool.h>

void progress_zero();
void progress_update();
void progress_skip(uint32_t count);

bool solution_verify(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void solution_found(uint32_t a, uint32_t b, uint32_t c, uint32_t d, bool sur);

extern const uint32_t max;

static inline uint32_t calc_dmax(uint32_t c)
{
	/* d must be odd */
	return (85 * c / 64) | 1;
}

static inline uint32_t calc_dmin(uint32_t c)
{
	/* d must be odd, c must be even */
	return c + 3;
}

static inline __uint128_t purec_pow4b(uint64_t x)
{
	uint64_t y = (uint64_t) x * x;
	return (__uint128_t) y * y;
}

static inline uint32_t purec_pow4i(uint32_t x)
{
	uint32_t y = x * x;
	return y * y;
}

static inline __uint128_t asm_pow4b(uint32_t x)
{
	/*
	 * The pure-C version of this is a few % faster than the ASM version.
	 * I suspect that with the function inlining, the inline-asm in this
	 * function presents an optimisation barrier to GCC.
	 */
	union {
		__uint128_t u128;
		uint64_t u64[2];
	} ret;
	/*
	 * Taking input in RDX then moving to RAX is faster than taking argument
	 * in RAX - TODO check GCC's generated ASM for calling code to see why
	 */
	asm(
		"mov rax, rdx\n"
		"mul rax\n"
		"mul rax\n"
		: "=a" (ret.u64[0]), "=d" (ret.u64[1])
		: "d" (x)
		:
		);
	return ret.u128;
}

static inline uint32_t asm_pow4i(uint32_t x)
{
	/*
	 * Around 1% faster than pure-C version, not significant (could be
	 * experimental error).
	 */
	asm (
		"mul eax\n"
		"mul eax\n"
		: "=a" (x)
		: "a" (x)
		:
		);
	return x;
}

#define pow4i asm_pow4i
#define pow4b purec_pow4b

static uint32_t euclid_gcd(uint32_t x, uint32_t y)
{
	if (x == 0) {
		return y;
	}
	if (y == 0) {
		return x;
	}
	uint8_t p2;
	for (p2 = 0; ((x | y) & 1) == 0; p2++) {
		x >>= 1;
		y >>= 1;
	}
	while ((x & 1) == 0) {
		x >>= 1;
	}
	do {
		while ((y & 1) == 0) {
			y >>= 1;
		}
		if (x > y) {
			uint32_t z = x;
			x = y;
			y = z;
		}
		y -= x;
	} while (y > 0);
	return x << p2;
}
