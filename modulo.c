#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include "main.h"
#include "modulo.h"

void using_modulo()
{
	progress_zero();
	for (uint32_t a = 1; a < max; a += 2) {
		const uint32_t a4 = pow4i(a);
#		pragma omp parallel for
		for (uint32_t b = a + 1; b < max; b += 2) {
			const uint32_t a4b4 = pow4i(b) + a4;
			progress_update();
			for (uint32_t c = b; c < max; c += 2) {
				const uint32_t a4b4c4 = pow4i(c) + a4b4;
				const uint32_t d = linear_search(a4b4c4, a, b, c);
				if (d) {
					solution_found(a, b, c, d, false);
				}
			}
		}
	}
}

uint32_t linear_search(uint32_t a4b4c4, uint32_t a, uint32_t b, uint32_t c)
{
	const uint32_t dmax = calc_dmax(c);
	const uint32_t dmin = calc_dmin(c);
	for (uint32_t d = dmin; d <= dmax; d += 2) {
		const uint32_t d4 = pow4i(d);
		/*
		 * Verify is necessary as one search may find multiple
		 * false-positives and we can only return one value
		 */
		if (d4 == a4b4c4 && solution_verify(a, b, c, d)) {
			return d;
		}
	}
	return 0;
}
