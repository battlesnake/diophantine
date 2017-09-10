#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "main.h"
#include "estimate_root.h"

void using_estimate_root()
{
	progress_zero();
	for (uint32_t a = 1; a < max; a += 2) {
		const __uint128_t a4 = pow4b(a);
#		pragma omp parallel for
		for (uint32_t b = a + 1; b < max; b += 2) {
			const uint32_t gcd = euclid_gcd(a, b);
			const __uint128_t a4b4 = pow4b(b) + a4;
			progress_update((max - b) / 2);
			for (uint32_t c = b; c < max; c += 2) {
				/* Skip if (a,b,c) != 1 */
				if (gcd > 1 && c % gcd == 0) {
					continue;
				}
				const __uint128_t a4b4c4 = pow4b(c) + a4b4;
				const uint32_t d = estimate_root(powf(a4b4c4, 0.25), a4b4c4);
				solution_found(a, b, c, d, false);
			}
		}
	}
}

static int64_t newton_rhapson(__int128_t y, int32_t x)
{
	uint64_t x2 = (uint64_t) x * x;
	__int128_t x3 = (__int128_t) x2 * x;
	__int128_t x4 = x3 * x;
	return x + (y - x4) / (4 * x3);
}

uint32_t estimate_root(uint32_t guess, __uint128_t x4)
{
	int32_t d0 = guess;
	/* printf("Estimate 4th root of %lu with initial guess x = %lu\n", (long) x4, (long) guess); */
	for (int iter = 0; iter < 100; iter++) {
		int32_t d1 = newton_rhapson(x4, d0);
		/* printf(" * %lu\n", (long) d1); */
		if (d0 == d1) {
			if (pow4b(d0) == x4) {
				/* printf(" SOLVED\n"); */
				return d0;
			} else if (pow4b(d0 + 1) == x4) {
				/* printf(" SOLVED\n"); */
				return d0 + 1;
			} else if (pow4b(d0 - 1) == x4) {
				/* printf(" SOLVED\n"); */
				return d0 - 1;
			}
			return 0;
		}
		d0 = d1;
	}
	/* printf(" FAILED\n"); */
	return 0;
}
