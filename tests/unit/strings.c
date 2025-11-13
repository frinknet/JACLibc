/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <strings.h>

TEST_TYPE(unit)
TEST_UNIT(strings.h)

/* ============================================================================
 * strcasecmp() TESTS
 * ============================================================================ */
TEST_SUITE(strcasecmp)

TEST(strcasecmp_equal) {
	ASSERT_EQ(0, strcasecmp("Hello", "HELLO"));
	ASSERT_EQ(0, strcasecmp("test", "TEST"));
	ASSERT_EQ(0, strcasecmp("MiXeD", "mixed"));
	ASSERT_EQ(0, strcasecmp("", ""));
	ASSERT_EQ(0, strcasecmp("ABC123", "abc123"));
}

TEST(strcasecmp_less) {
	ASSERT_LT(strcasecmp("abc", "xyz"), 0);
	ASSERT_LT(strcasecmp("Apple", "BANANA"), 0);
	ASSERT_LT(strcasecmp("test", "zebra"), 0);
	ASSERT_LT(strcasecmp("", "test"), 0);
}

TEST(strcasecmp_greater) {
	ASSERT_GT(strcasecmp("xyz", "abc"), 0);
	ASSERT_GT(strcasecmp("ZEBRA", "apple"), 0);
	ASSERT_GT(strcasecmp("banana", "Apple"), 0);
	ASSERT_GT(strcasecmp("test", ""), 0);
}

TEST(strcasecmp_single_char) {
	ASSERT_EQ(0, strcasecmp("a", "A"));
	ASSERT_EQ(0, strcasecmp("Z", "z"));
	ASSERT_LT(strcasecmp("a", "B"), 0);
	ASSERT_GT(strcasecmp("Z", "a"), 0);
}

TEST(strcasecmp_with_numbers) {
	ASSERT_EQ(0, strcasecmp("test123", "TEST123"));
	ASSERT_LT(strcasecmp("1test", "2test"), 0);
	ASSERT_GT(strcasecmp("test9", "test1"), 0);
}

TEST(strcasecmp_special_chars) {
	ASSERT_EQ(0, strcasecmp("@#$", "@#$"));
	ASSERT_LT(strcasecmp("!", "@"), 0);
	ASSERT_EQ(0, strcasecmp("Test123!@#", "TEST123!@#"));
}

TEST(strcasecmp_different_lengths) {
	ASSERT_LT(strcasecmp("abc", "abcd"), 0);
	ASSERT_GT(strcasecmp("abcd", "abc"), 0);
}

TEST(strcasecmp_null) {
	ASSERT_EQ(0, strcasecmp(NULL, NULL));
	ASSERT_GT(strcasecmp("test", NULL), 0);
	ASSERT_LT(strcasecmp(NULL, "test"), 0);
}

/* ============================================================================
 * strncasecmp() TESTS
 * ============================================================================ */
TEST_SUITE(strncasecmp)

TEST(strncasecmp_equal) {
	ASSERT_EQ(0, strncasecmp("Hello", "HELLO", 5));
	ASSERT_EQ(0, strncasecmp("test123", "TEST456", 4));
	ASSERT_EQ(0, strncasecmp("PREFIX_diff", "prefix_other", 6));
	ASSERT_EQ(0, strncasecmp("abc", "ABC", 3));
}

TEST(strncasecmp_less) {
	ASSERT_LT(strncasecmp("abcd", "AXCD", 2), 0);
	ASSERT_LT(strncasecmp("test", "TXYZ", 2), 0);
	ASSERT_LT(strncasecmp("aaa", "AAB", 3), 0);
}

TEST(strncasecmp_greater) {
	ASSERT_GT(strncasecmp("ZEBRA", "apple", 1), 0);
	ASSERT_GT(strncasecmp("xyz", "ABC", 1), 0);
	ASSERT_GT(strncasecmp("bbb", "BBA", 3), 0);
}

TEST(strncasecmp_zero_length) {
	ASSERT_EQ(0, strncasecmp("anything", "different", 0));
	ASSERT_EQ(0, strncasecmp("", "", 0));
	ASSERT_EQ(0, strncasecmp("abc", "xyz", 0));
}

TEST(strncasecmp_partial) {
	ASSERT_EQ(0, strncasecmp("Hello World", "HELLO EARTH", 5));
	ASSERT_EQ(0, strncasecmp("test", "TESTX", 4));
	ASSERT_EQ(0, strncasecmp("prefix123", "PREFIX999", 6));
}

TEST(strncasecmp_shorter_strings) {
	ASSERT_EQ(0, strncasecmp("Hi", "HI", 10));
	ASSERT_EQ(0, strncasecmp("short", "SHORT_LONG", 5));
	ASSERT_EQ(0, strncasecmp("abc", "abcdef", 3));
}

TEST(strncasecmp_exact_length) {
	ASSERT_EQ(0, strncasecmp("test", "TEST", 4));
	ASSERT_EQ(0, strncasecmp("test", "TESTX", 4));
}

TEST(strncasecmp_null) {
	ASSERT_EQ(0, strncasecmp(NULL, NULL, 5));
	ASSERT_GT(strncasecmp("test", NULL, 4), 0);
	ASSERT_LT(strncasecmp(NULL, "test", 4), 0);
	ASSERT_EQ(0, strncasecmp(NULL, NULL, 0));
}

/* ============================================================================
 * ffs() TESTS
 * ============================================================================ */
TEST_SUITE(ffs)

TEST(ffs_zero) {
	ASSERT_EQ(0, ffs(0));
}

TEST(ffs_lowest_bit) {
	ASSERT_EQ(1, ffs(0x00000001));
	ASSERT_EQ(1, ffs(0xFFFFFFFF));
	ASSERT_EQ(1, ffs(0x00000003));
	ASSERT_EQ(1, ffs(0x00000007));
}

TEST(ffs_single_bits) {
	ASSERT_EQ(1, ffs(0x00000001));
	ASSERT_EQ(2, ffs(0x00000002));
	ASSERT_EQ(3, ffs(0x00000004));
	ASSERT_EQ(4, ffs(0x00000008));
	ASSERT_EQ(8, ffs(0x00000080));
	ASSERT_EQ(16, ffs(0x00008000));
	ASSERT_EQ(32, ffs(0x80000000));
}

TEST(ffs_multiple_bits) {
	ASSERT_EQ(2, ffs(0x00000006));
	ASSERT_EQ(3, ffs(0x0000000C));
	ASSERT_EQ(4, ffs(0x00000018));
}

TEST(ffs_powers_of_two) {
	for (int i = 0; i < 32; i++) {
		ASSERT_EQ(i + 1, ffs(1 << i));
	}
}

TEST(ffs_patterns) {
	ASSERT_EQ(2, ffs(0xAAAAAAAA));
	ASSERT_EQ(1, ffs(0x55555555));
}

TEST(ffs_sparse_bits) {
	ASSERT_EQ(5, ffs(0x00000010));
	ASSERT_EQ(9, ffs(0x00000100));
	ASSERT_EQ(17, ffs(0x00010000));
	ASSERT_EQ(25, ffs(0x01000000));
}

/* ============================================================================
 * ffsl() TESTS
 * ============================================================================ */
TEST_SUITE(ffsl)

TEST(ffsl_zero) {
	ASSERT_EQ(0, ffsl(0L));
}

TEST(ffsl_basic) {
	ASSERT_EQ(1, ffsl(1L));
	ASSERT_EQ(2, ffsl(2L));
	ASSERT_EQ(32, ffsl(0x80000000L));
}

#if JACL_64BIT
TEST(ffsl_high_bits) {
	ASSERT_EQ(33, ffsl(0x100000000L));
	ASSERT_EQ(40, ffsl(0x8000000000L));
	ASSERT_EQ(64, ffsl(0x8000000000000000L));
}
#endif

/* ============================================================================
 * ffsll() TESTS
 * ============================================================================ */
TEST_SUITE(ffsll)

TEST(ffsll_zero) {
	ASSERT_EQ(0, ffsll(0LL));
}

TEST(ffsll_low_bits) {
	ASSERT_EQ(1, ffsll(1LL));
	ASSERT_EQ(2, ffsll(2LL));
	ASSERT_EQ(16, ffsll(0x8000LL));
}

TEST(ffsll_high_bits) {
	ASSERT_EQ(33, ffsll(0x100000000LL));
	ASSERT_EQ(40, ffsll(0x8000000000LL));
	ASSERT_EQ(64, ffsll(0x8000000000000000LL));
}

TEST(ffsll_all_bits) {
	ASSERT_EQ(1, ffsll(0xFFFFFFFFFFFFFFFFLL));
}

/* ============================================================================
 * fls() TESTS
 * ============================================================================ */
TEST_SUITE(fls)

TEST(fls_zero) {
	ASSERT_EQ(0, fls(0));
}

TEST(fls_highest_bit) {
	ASSERT_EQ(32, fls(0x80000000));
	ASSERT_EQ(32, fls(0xFFFFFFFF));
	ASSERT_EQ(32, fls(0xC0000000));
	ASSERT_EQ(32, fls(0xE0000000));
}

TEST(fls_single_bits) {
	ASSERT_EQ(1, fls(0x00000001));
	ASSERT_EQ(2, fls(0x00000002));
	ASSERT_EQ(3, fls(0x00000004));
	ASSERT_EQ(4, fls(0x00000008));
	ASSERT_EQ(16, fls(0x00008000));
	ASSERT_EQ(32, fls(0x80000000));
}

TEST(fls_multiple_bits) {
	ASSERT_EQ(2, fls(0x00000003));
	ASSERT_EQ(3, fls(0x00000007));
	ASSERT_EQ(3, fls(0x00000006));
	ASSERT_EQ(4, fls(0x0000000F));
}

TEST(fls_powers_of_two) {
	for (int i = 0; i < 32; i++) {
		ASSERT_EQ(i + 1, fls(1 << i));
	}
}

TEST(fls_patterns) {
	ASSERT_EQ(32, fls(0xAAAAAAAA));
	ASSERT_EQ(31, fls(0x55555555));
}

TEST(fls_sparse_bits) {
	ASSERT_EQ(5, fls(0x00000010));
	ASSERT_EQ(9, fls(0x00000100));
	ASSERT_EQ(17, fls(0x00010000));
	ASSERT_EQ(25, fls(0x01000000));
}

/* ============================================================================
 * flsl() TESTS
 * ============================================================================ */
TEST_SUITE(flsl)

TEST(flsl_zero) {
	ASSERT_EQ(0, flsl(0L));
}

TEST(flsl_basic) {
	ASSERT_EQ(1, flsl(1L));
	ASSERT_EQ(2, flsl(2L));
	ASSERT_EQ(32, flsl(0x80000000L));
}

#if JACL_64BIT
TEST(flsl_high_bits) {
	ASSERT_EQ(33, flsl(0x100000000L));
	ASSERT_EQ(40, flsl(0x8000000000L));
	ASSERT_EQ(64, flsl(0x8000000000000000L));
}
#endif

/* ============================================================================
 * flsll() TESTS
 * ============================================================================ */
TEST_SUITE(flsll)

TEST(flsll_zero) {
	ASSERT_EQ(0, flsll(0LL));
}

TEST(flsll_low_bits) {
	ASSERT_EQ(1, flsll(1LL));
	ASSERT_EQ(2, flsll(2LL));
	ASSERT_EQ(16, flsll(0x8000LL));
}

TEST(flsll_high_bits) {
	ASSERT_EQ(33, flsll(0x100000000LL));
	ASSERT_EQ(40, flsll(0x8000000000LL));
	ASSERT_EQ(64, flsll(0x8000000000000000LL));
}

TEST(flsll_all_bits) {
	ASSERT_EQ(64, flsll(0xFFFFFFFFFFFFFFFFLL));
}

/* ============================================================================
 * bzero() TESTS
 * ============================================================================ */
TEST_SUITE(bzero)

TEST(bzero_partial) {
	char buffer[10] = "test1234";
	bzero(buffer, 4);
	
	ASSERT_EQ(0, buffer[0]);
	ASSERT_EQ(0, buffer[1]);
	ASSERT_EQ(0, buffer[2]);
	ASSERT_EQ(0, buffer[3]);
	ASSERT_EQ('1', buffer[4]);
}

TEST(bzero_full) {
	int array[5] = {1, 2, 3, 4, 5};
	bzero(array, sizeof(array));
	
	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(0, array[i]);
	}
}

TEST(bzero_zero_length) {
	char buffer[5] = "test";
	
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsuspicious-bzero"
	bzero(buffer, 0);
	#pragma GCC diagnostic pop
	
	ASSERT_EQ('t', buffer[0]);
}

TEST(bzero_struct) {
	struct { int a; char b; } s = {42, 'X'};
	bzero(&s, sizeof(s));
	
	ASSERT_EQ(0, s.a);
	ASSERT_EQ(0, s.b);
}

/* ============================================================================
 * bcopy() TESTS
 * ============================================================================ */
TEST_SUITE(bcopy)

TEST(bcopy_basic) {
	char src[6] = "hello";
	char dest[6] = {0};
	bcopy(src, dest, 5);
	
	ASSERT_STR_EQ("hello", dest);
}

TEST(bcopy_overlap_forward) {
	char buffer[10] = "abcdefgh";
	bcopy(buffer, buffer + 2, 4);
	
	ASSERT_EQ('a', buffer[2]);
	ASSERT_EQ('b', buffer[3]);
	ASSERT_EQ('c', buffer[4]);
	ASSERT_EQ('d', buffer[5]);
}

TEST(bcopy_overlap_backward) {
	char buffer[10] = "abcdefgh";
	bcopy(buffer + 2, buffer, 4);
	
	ASSERT_EQ('c', buffer[0]);
	ASSERT_EQ('d', buffer[1]);
	ASSERT_EQ('e', buffer[2]);
	ASSERT_EQ('f', buffer[3]);
}

TEST(bcopy_zero_length) {
	char src[5] = "test";
	char dest[5] = "orig";
	bcopy(src, dest, 0);
	
	ASSERT_STR_EQ("orig", dest);
}

/* ============================================================================
 * bcmp() TESTS
 * ============================================================================ */
TEST_SUITE(bcmp)

TEST(bcmp_equal) {
	char buf1[5] = "test";
	char buf2[5] = "test";
	
	ASSERT_EQ(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_different) {
	char buf1[5] = "test";
	char buf2[5] = "TEXT";
	
	ASSERT_NE(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_partial_equal) {
	char buf1[6] = "hello";
	char buf2[6] = "help!";
	
	ASSERT_EQ(0, bcmp(buf1, buf2, 3));
}

TEST(bcmp_partial_different) {
	char buf1[6] = "hello";
	char buf2[6] = "help!";
	
	ASSERT_NE(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_binary_equal) {
	unsigned char a[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char b[4] = {0xFF, 0x00, 0xAA, 0x55};
	
	ASSERT_EQ(0, bcmp(a, b, 4));
}

TEST(bcmp_binary_different) {
	unsigned char a[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char c[4] = {0xFF, 0x00, 0xBB, 0x55};
	
	ASSERT_NE(0, bcmp(a, c, 4));
}

TEST(bcmp_zero_length) {
	char buf1[5] = "test";
	char buf2[5] = "diff";
	
	ASSERT_EQ(0, bcmp(buf1, buf2, 0));
}

/* ============================================================================
 * index() TESTS
 * ============================================================================ */
TEST_SUITE(index)

TEST(index_found_first) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 0, index(str, 'h'));
}

TEST(index_found_middle) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 4, index(str, 'o'));
	ASSERT_EQ(str + 6, index(str, 'w'));
}

TEST(index_found_last) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 10, index(str, 'd'));
}

TEST(index_not_found) {
	const char *str = "hello";
	
	ASSERT_NULL(index(str, 'z'));
	ASSERT_NULL(index(str, 'X'));
}

TEST(index_null_terminator) {
	const char *str = "test";
	
	ASSERT_EQ(str + 4, index(str, '\0'));
}

TEST(index_first_of_many) {
	const char *str = "banana";
	
	ASSERT_EQ(str + 1, index(str, 'a'));
	ASSERT_EQ(str + 2, index(str, 'n'));
}

/* ============================================================================
 * rindex() TESTS
 * ============================================================================ */
TEST_SUITE(rindex)

TEST(rindex_found_last) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 7, rindex(str, 'o'));
	ASSERT_EQ(str + 9, rindex(str, 'l'));
}

TEST(rindex_found_only) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 0, rindex(str, 'h'));
	ASSERT_EQ(str + 10, rindex(str, 'd'));
}

TEST(rindex_not_found) {
	const char *str = "hello";
	
	ASSERT_NULL(rindex(str, 'z'));
	ASSERT_NULL(rindex(str, 'X'));
}

TEST(rindex_last_of_many) {
	const char *str = "banana";
	
	ASSERT_EQ(str + 5, rindex(str, 'a'));
	ASSERT_EQ(str + 4, rindex(str, 'n'));
}

TEST(rindex_single_char) {
	const char *str = "hello";
	
	ASSERT_EQ(str + 0, rindex(str, 'h'));
	ASSERT_EQ(str + 1, rindex(str, 'e'));
}

TEST_MAIN()

