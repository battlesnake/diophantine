#include <stdio.h>
#include <math.h>
#include <stdint.h>
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
			const __uint128_t a4b4 = pow4b(b) + a4;
			progress_update();
			for (uint32_t c = b; c < max; c += 2) {
				const __uint128_t a4b4c4 = pow4b(c) + a4b4;
				const uint32_t d = estimate_root(a4b4c4);
				solution_found(a, b, c, d, false);
			}
		}
	}
}

uint32_t estimate_root(__uint128_t a4b4c4)
{
	/* TODO: Newton's method instead */
	return powl(a4b4c4, 0.25);
}
