/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdarg.h>

TEST_TYPE(unit);
TEST_UNIT(stdarg.h);

/* ============================================================================
 * HELPER VARIADIC FUNCTIONS
 * ============================================================================ */

static int sum_ints(int count, ...) {
	va_list args;
	va_start(args, count);

	int sum = 0;
	for (int i = 0; i < count; i++) {
		sum += va_arg(args, int);
	}

	va_end(args);
	return sum;
}

static double sum_doubles(int count, ...) {
	va_list args;
	va_start(args, count);

	double sum = 0.0;
	for (int i = 0; i < count; i++) {
		sum += va_arg(args, double);
	}

	va_end(args);
	return sum;
}

static char first_char(int count, ...) {
	va_list args;
	va_start(args, count);

	char result = '\0';
	if (count > 0) {
		result = (char)va_arg(args, int);  // char promoted to int
	}

	va_end(args);
	return result;
}

static int find_max(int count, ...) {
	va_list args;
	va_start(args, count);

	int max = va_arg(args, int);
	for (int i = 1; i < count; i++) {
		int val = va_arg(args, int);
		if (val > max) max = val;
	}

	va_end(args);
	return max;
}

static int count_above_threshold(int threshold, int count, ...) {
	va_list args;
	va_start(args, count);

	int result = 0;
	for (int i = 0; i < count; i++) {
		if (va_arg(args, int) > threshold) {
			result++;
		}
	}

	va_end(args);
	return result;
}

/* ============================================================================
 * BASIC VARIADIC TESTS
 * ============================================================================ */
TEST_SUITE(basic_variadic);

TEST(sum_two_ints) {
	int result = sum_ints(2, 10, 20);
	ASSERT_EQ(30, result);
}

TEST(sum_five_ints) {
	int result = sum_ints(5, 1, 2, 3, 4, 5);
	ASSERT_EQ(15, result);
}

TEST(sum_zero_ints) {
	int result = sum_ints(0);
	ASSERT_EQ(0, result);
}

TEST(sum_one_int) {
	int result = sum_ints(1, 42);
	ASSERT_EQ(42, result);
}

/* ============================================================================
 * DIFFERENT TYPE TESTS
 * ============================================================================ */
TEST_SUITE(different_types);

TEST(sum_doubles) {
	double result = sum_doubles(3, 1.5, 2.5, 3.0);
	ASSERT_DBL_NEAR(7.0, result, 0.001);
}

TEST(char_argument) {
	char result = first_char(1, 'A');
	ASSERT_EQ('A', result);
}

TEST(mixed_integer_sizes) {
	int result = sum_ints(4, 100, 200, 300, 400);
	ASSERT_EQ(1000, result);
}

/* ============================================================================
 * VA_COPY TESTS
 * ============================================================================ */
TEST_SUITE(va_copy);

static int sum_twice(int count, ...) {
	va_list args1, args2;
	va_start(args1, count);
	va_copy(args2, args1);

	int sum1 = 0;
	for (int i = 0; i < count; i++) {
		sum1 += va_arg(args1, int);
	}

	int sum2 = 0;
	for (int i = 0; i < count; i++) {
		sum2 += va_arg(args2, int);
	}

	va_end(args1);
	va_end(args2);

	return sum1 + sum2;
}

TEST(va_copy_basic) {
	int result = sum_twice(3, 10, 20, 30);
	ASSERT_EQ(120, result);  // (10+20+30) * 2
}

/* ============================================================================
 * MULTIPLE TRAVERSALS
 * ============================================================================ */
TEST_SUITE(multiple_traversals);

static int count_and_sum(int count, int *out_count, ...) {
	va_list args;
	va_start(args, out_count);

	*out_count = count;
	int sum = 0;
	for (int i = 0; i < count; i++) {
		sum += va_arg(args, int);
	}

	va_end(args);
	return sum;
}

TEST(count_and_sum_together) {
	int count;
	int sum = count_and_sum(4, &count, 1, 2, 3, 4);

	ASSERT_EQ(4, count);
	ASSERT_EQ(10, sum);
}

/* ============================================================================
 * COMPLEX SCENARIOS
 * ============================================================================ */
TEST_SUITE(complex_scenarios);

TEST(find_maximum) {
	int max = find_max(5, 10, 50, 30, 90, 20);
	ASSERT_EQ(90, max);
}

TEST(threshold_counting) {
	int count = count_above_threshold(50, 6, 10, 60, 30, 80, 40, 100);
	ASSERT_EQ(3, count);  // 60, 80, 100
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(single_argument) {
	int result = sum_ints(1, 999);
	ASSERT_EQ(999, result);
}

TEST(many_arguments) {
	int result = sum_ints(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	ASSERT_EQ(55, result);
}

TEST(negative_numbers) {
	int result = sum_ints(3, -10, -20, -30);
	ASSERT_EQ(-60, result);
}

TEST(zero_values) {
	int result = sum_ints(4, 0, 0, 0, 0);
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * PRACTICAL PATTERNS
 * ============================================================================ */
TEST_SUITE(practical_patterns);

static int printf_style_function(const char *format, ...) {
	va_list args;
	va_start(args, format);

	int count = 0;
	while (*format) {
		if (*format == '%' && *(format + 1) == 'd') {
			(void)va_arg(args, int);
			count++;
			format++;
		}
		format++;
	}

	va_end(args);
	return count;
}

TEST(printf_style_parsing) {
	int count = printf_style_function("Value: %d, Count: %d", 42, 100);
	ASSERT_EQ(2, count);
}

TEST_MAIN()
