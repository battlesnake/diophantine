#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include "main.h"
#include "binary_search.h"

void using_binary_search()
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
				const uint32_t d = binary_search(a4b4c4, calc_dmin(c), calc_dmax(c));
				if (d) {
					solution_found(a, b, c, d, true);
				}
			}
		}
	}
}

uint32_t binary_search(__uint128_t a4b4c4, uint32_t dmin, uint32_t dmax)
{
	int64_t d = (dmin + dmax) / 2;
	int64_t dd = (1 + dmax - dmin) / 2;
	while (dd > 0) {
		const __uint128_t d4 = pow4b(d);
		const int64_t e = d4 > a4b4c4 ? -dd : d4 < a4b4c4 ? dd : 0;
		if (e == 0) {
			return d;
		}
		d += e;
		dd /= 2;
	}
	return 0;
}
