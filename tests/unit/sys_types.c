/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/types.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(sys/types.h);

/* ============================================================================
 * TYPE EXISTENCE
 * ============================================================================ */
TEST_SUITE(type_existence);

TEST(time_t_exists) {
	time_t t = 0;
	ASSERT_EQ(0, t);
}

TEST(clock_t_exists) {
	clock_t c = 0;
	ASSERT_EQ(0, c);
}

TEST(off_t_exists) {
	off_t o = 0;
	ASSERT_EQ(0, o);
}

TEST(pid_t_exists) {
	pid_t p = 0;
	ASSERT_EQ(0, p);
}

TEST(uid_t_exists) {
	uid_t u = 0;
	ASSERT_EQ(0, u);
}

TEST(gid_t_exists) {
	gid_t g = 0;
	ASSERT_EQ(0, g);
}

TEST(mode_t_exists) {
	mode_t m = 0;
	ASSERT_EQ(0, m);
}

TEST(ino_t_exists) {
	ino_t i = 0;
	ASSERT_EQ(0, i);
}

TEST(dev_t_exists) {
	dev_t d = 0;
	ASSERT_EQ(0, d);
}

TEST(ssize_t_exists) {
	ssize_t s = 0;
	ASSERT_EQ(0, s);
}

/* ============================================================================
 * TYPE SIZES
 * ============================================================================ */
TEST_SUITE(type_sizes);

TEST(time_t_size) {
	ASSERT_TRUE(sizeof(time_t) >= sizeof(long));
}

TEST(off_t_size) {
	// off_t should be at least 32-bit, often 64-bit
	ASSERT_TRUE(sizeof(off_t) >= 4);
}

TEST(pid_t_size) {
	ASSERT_EQ(sizeof(int), sizeof(pid_t));
}

TEST(uid_t_size) {
	ASSERT_EQ(sizeof(unsigned int), sizeof(uid_t));
}

TEST(gid_t_size) {
	ASSERT_EQ(sizeof(unsigned int), sizeof(gid_t));
}

TEST(ssize_t_size) {
	// ssize_t should match size_t size
	ASSERT_EQ(sizeof(size_t), sizeof(ssize_t));
}

/* ============================================================================
 * TYPE SIGNEDNESS
 * ============================================================================ */
TEST_SUITE(type_signedness);

TEST(time_t_signed) {
	time_t negative = -1;
	ASSERT_TRUE(negative < 0);
}

TEST(pid_t_signed) {
	pid_t negative = -1;
	ASSERT_TRUE(negative < 0);
}

TEST(ssize_t_signed) {
	ssize_t negative = -1;
	ASSERT_TRUE(negative < 0);
}

TEST(uid_t_unsigned) {
	uid_t u = (uid_t)-1;
	ASSERT_TRUE(u > 0);
}

TEST(gid_t_unsigned) {
	gid_t g = (gid_t)-1;
	ASSERT_TRUE(g > 0);
}

TEST(mode_t_unsigned) {
	mode_t m = (mode_t)-1;
	ASSERT_TRUE(m > 0);
}

/* ============================================================================
 * TYPE RANGES
 * ============================================================================ */
TEST_SUITE(type_ranges);

TEST(pid_t_range) {
	pid_t max = INT_MAX;
	pid_t min = INT_MIN;
	
	ASSERT_TRUE(max > 0);
	ASSERT_TRUE(min < 0);
}

TEST(off_t_large_values) {
	off_t large = 1LL << 32;  // > 4GB
	ASSERT_TRUE(large > 0);
}

TEST(time_t_range) {
	time_t t = 0;
	t = t + 1000000000;  // ~31 years in seconds
	ASSERT_TRUE(t > 0);
}

/* ============================================================================
 * TYPE COMPATIBILITY
 * ============================================================================ */
TEST_SUITE(type_compatibility);

TEST(pid_t_compatible_with_int) {
	pid_t p = 12345;
	int i = (int)p;
	
	ASSERT_EQ(12345, i);
}

TEST(uid_gid_compatible) {
	uid_t u = 1000;
	gid_t g = (gid_t)u;
	
	ASSERT_EQ(1000, g);
}

TEST(ssize_t_size_t_compatible) {
	size_t sz = 100;
	ssize_t ssz = (ssize_t)sz;
	
	ASSERT_EQ(100, ssz);
}

/* ============================================================================
 * SPECIAL VALUES
 * ============================================================================ */
TEST_SUITE(special_values);

TEST(pid_t_special_values) {
	pid_t init = 1;
	pid_t invalid = -1;
	
	ASSERT_EQ(1, init);
	ASSERT_EQ(-1, invalid);
}

TEST(off_t_zero) {
	off_t zero = 0;
	ASSERT_EQ(0, zero);
}

TEST(mode_t_zero) {
	mode_t none = 0;
	ASSERT_EQ(0, none);
}

/* ============================================================================
 * LARGE FILE SUPPORT
 * ============================================================================ */
#if JACL_HAS_LFS
TEST_SUITE(large_file_support);

TEST(off64_t_exists) {
	off64_t o = 0;
	ASSERT_EQ(0, o);
}

TEST(off64_t_size) {
	ASSERT_EQ(8, sizeof(off64_t));
}

TEST(off64_t_large_values) {
	off64_t large = 5LL * 1024 * 1024 * 1024;  // 5GB
	ASSERT_TRUE(large > INT_MAX);
}

TEST(blkcnt64_t_exists) {
	blkcnt64_t b = 0;
	ASSERT_EQ(0, b);
}
#endif

/* ============================================================================
 * ARITHMETIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(arithmetic_operations);

TEST(pid_t_arithmetic) {
	pid_t p = 100;
	p = p + 50;
	
	ASSERT_EQ(150, p);
}

TEST(off_t_arithmetic) {
	off_t o = 1000;
	o = o * 2;
	
	ASSERT_EQ(2000, o);
}

TEST(ssize_t_arithmetic) {
	ssize_t s = 100;
	s = s - 50;
	
	ASSERT_EQ(50, s);
}

/* ============================================================================
 * COMPARISONS
 * ============================================================================ */
TEST_SUITE(comparisons);

TEST(pid_t_comparison) {
	pid_t p1 = 100;
	pid_t p2 = 200;
	
	ASSERT_TRUE(p1 < p2);
	ASSERT_TRUE(p2 > p1);
	ASSERT_FALSE(p1 == p2);
}

TEST(off_t_comparison) {
	off_t o1 = 1000;
	off_t o2 = 2000;
	
	ASSERT_TRUE(o1 < o2);
}

TEST(uid_gid_comparison) {
	uid_t u1 = 100;
	uid_t u2 = 100;
	
	ASSERT_TRUE(u1 == u2);
}

TEST_MAIN()
