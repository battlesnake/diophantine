#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "main.h"
#include "modulo.h"
#include "binary_search.h"
#include "estimate_root.h"

const uint32_t max = 500000;

static void test()
{
	const uint32_t x = 0xfedcba98U;
	if (pow4b(1) != 1) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "pow4b");
	}
	if (pow4b(x) != (__uint128_t) x * x * x * x) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "pow4b");
	}
	if (pow4i(1) != 1) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "pow4i");
	}
	if (pow4i(x) != x * x * x * x) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "pow4i");
	}
	if (!solution_verify(95800, 217519, 414560, 422481)) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "solution_verify");
	}
	if (solution_verify(95800, 217519, 414560, 422482)) {
		fprintf(stderr, "%d: %s broken\n", __LINE__, "solution_verify");
	}
}

int main(int argc, char *argv[])
{
	/*
	 * Finds some solutions to the Diophantine equation:
	 *
	 *   a⁴ + b⁴ + c⁴ = d⁴
	 *
	 * where:
	 *
	 *   a, b, c, d ∊ ℤ⁺
	 *
	 */

	/*
	 * We introduce some constraints and optimisations, to prune the search
	 * space.  Some of these will eliminate valid solutions.
	 */

	/*
	 * 1. Cache powers at start of loop a4 b4 c4 d4 and intermediate sums.
	 */

	/*
	 * 2. No need to re-test permutations of values for a, b, c since
	 *   integer addition commutes.
	 *
	 *  ∴ Constraints:
	 *   ∙ a ≤ b ≤ c
	 */

	/*
	 * 3. x⁴ (x ∊ ℤ⁺) in base-2 ends with either:
	 *     ∙ 0000 (x even)
	 *     ∙ 0001 (x odd)
	 *
	 *  This means that a⁴ + b⁴ + c⁴ must end with either of those.
	 *  So a, b, c are all even, or exactly one of them is odd.
	 *
	 *  If a, b, c are all even, then they share a common factor so we can
	 *  skip testing when a, b, c all even (since we can generate such
	 *  solutions from the solutions where a is odd).
	 *
	 *  ∴ Constraints:
	 *   ∙ a, d: odd
	 *   ∙ b, c: even
	 *
	 * This loses some solutions, as it requires that "a" (the smallest
	 * value of a/b/c) is also the odd one.
	 */

	/*
	 * 2. a, b, c, d ∊ ℤ⁼
	 *
	 *  ∴ Constraints:
	 *   ∙ d > max(a, b, c)
	 */

	/*
	 * 3. If a = b = c, the equation becomes d⁴ = 3c⁴.
	 *
	 *  ∴ Constraints:
	 *   ∙ d ≤ ⁴√3 · max(a, b, c)
	 *
	 *  - Micro-optimisation: ⁴√3 = 1.316074 ≤ 1.328215 = 85/64.
	 */

	/*
	 * Combining the constraints:
	 *   ∙ a: odd
	 *   ∙ b: even, > a
	 *   ∙ c: even, ≥ b
	 *   ∙ d: odd, > c, ≤ ⌈85.c/64⌉
	 */

	/*
	 * 4. Eliminating exponents?
	 *
	 *   We could also try to optimise the loops, to avoid calculting
	 *   hypercubics (quartics?) each iteration:
	 *
	 *   By binomial expansion:
	 *    ∙ squares:   xₙ = n², xₙ₊₁ = xₙ + 2n + 1
	 *    ∙ cube:      xₙ = n³, xₙ₊₁ = xₙ + 3n² + 3n + 1
	 *    ∙ hypercube: xₙ = n³, xₙ₊₁ = xₙ + 4n³ + 6n² + 4n + 1
	 *
	 * I expect that the massive amout of extra additions required in order
	 * to calculate xₙ₊₁ from xₙ would be slower over-all than the two
	 * multiplications current required to calculate xₙ₊₁ from n, so I did
	 * not bother to implement this.
	 */

	/*
	 * Range overflow
	 *
	 * We will rapidly exhaust the range of a 64-bit integer given that we
	 * are calculating 4th exponents.
	 *
	 * x⁴ will exceed the range when x > 65535.
	 *
	 * a) We can test quickly for solutions using a heuristic with modulo
	 *    arithmetic, then we can verify using slower big-integer
	 *    arithmetic.
	 *    This requires iterating over values of d, but allows us to do most
	 *    of the arithmetic using fast integer types that fit in CPU
	 *    registers and require a single CPU instruction to
	 *    multiply and add.
	 *
	 * b) We can use big-integer arithmetic to calculate the left-side, then
	 *    use a binary search to find the value of d such that:
	 *
	 *      | a⁴ + b⁴ + c⁴ - d⁴ |
	 *
	 *    is minimised.
	 *
	 *    If we find the minimum value is zero, then we have found a
	 *    solution to the original equation.
	 *
	 *    This should have lower asymptotic time complexity than (a) but
	 *    uses slower data types for the arithmetic, often requiring several
	 *    CPU instructions to multiply and add.
	 *
	 * c) We can use large integers as in (b) but instead of searching we
	 *    calculate the 4th root of a⁴ + b⁴ + c⁴ and see if it is integral.
	 *
	 *    This requires a performant and accurate square-root or 4th-root
	 *    function for large integers.  If the accuracy of the root function
	 *    is imperfect but with known error range then it can be used to
	 *    reduce the search space for a linear or binary search to follow.
	 *
	 *
	 * Option (b) should be faster than (a) since it uses a binary search
	 * with log time rather than an iterative search.  Heuristics and range
	 * estimates could possibly be used to speed up the modulo method in (a)
	 * though, by reducing the search space.
	 *
	 * I expect the following performance:
	 *
	 *  ∙ Binary search: space = O(1), time = O(N³logN)
	 *  ∙ Linear search: space = O(1), time = O(N⁴)
	 *
	 */

	test();

	if (argc <= 1 || strcmp(argv[1], "-h") == 0) {
		fprintf(stderr, "Argument required: modulo / binary / estimate\n");
		return EINVAL;
	} else if (strcmp(argv[1], "modulo") == 0) {
		using_modulo();
	} else if (strcmp(argv[1], "binary") == 0) {
		using_binary_search();
	} else if (strcmp(argv[1], "estimate") == 0) {
		using_estimate_root();
	} else {
		fprintf(stderr, "Invalid argument\n");
		return EINVAL;
	}

	return 0;
}

bool solution_verify(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
	const __uint128_t a4 = pow4b(a);
	const __uint128_t b4 = pow4b(b);
	const __uint128_t c4 = pow4b(c);
	const __uint128_t d4 = pow4b(d);
	return a4 + b4 + c4 == d4;
}

void solution_found(uint32_t a, uint32_t b, uint32_t c, uint32_t d, bool sure)
{
	const bool valid = solution_verify(a, b, c, d);
	if (!valid && !sure) {
		if (!sure) {
			return;
		}
		/* Handle false positives? */
	}
#	pragma omp critical
	{
		const char *result = valid ? "found" : "INVALID";
		fprintf(stderr, "\n");
		printf("Solution %s: %u⁴ + %u⁴ + %u⁴ = %u⁴\n", result, a, b, c, d);
	}
}

/* Progress monitoring */
static clock_t t0;
static __uint128_t prog_val;
static __uint128_t prog_max;

/*
 * Progress:
 *
 * A = 1, 3, ..., N
 * B = A+1, A+3, ..., N
 * C = B+2, B+4, ..., N
 *
 * Let n = N/2:
 *
 * A = 2a+1; a < n
 * B = 2b+1+A; b < n-a
 * C = 2c+2+B; c < n-b-a
 *
 * Approximate triple-summation with integral over a, b, c of 1.dc.db.da, to get
 * total cycles approx N^3/144.
 *
 */

void progress_zero()
{
	prog_val = 0;
	t0 = clock();
	prog_max = (__uint128_t) max * max * max / 144;
}

void progress_update(uint32_t delta)
{
	if (!(prog_val & 0x3ff)) {
		if (prog_val == 0) {
			t0 = clock();
		}
		const float dt = (clock() - t0) * 1.0f / CLOCKS_PER_SEC;
		const float eta = prog_max * dt / prog_val - dt;
		fprintf(stderr, "\r%.3f%%, ETA %.0fs (%.1fy)  ", prog_val * 100.0f / prog_max, eta, eta / (86400 * 365.25));
	}
#	pragma omp atomic
	prog_val += delta;
}

void progress_skip(uint32_t count)
{
	prog_max -= count;
}
