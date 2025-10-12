/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <strings.h>

TEST_TYPE(unit);
TEST_UNIT(strings.h);

/* ============================================================================
 * CASE-INSENSITIVE COMPARISON TESTS
 * ============================================================================ */
TEST_SUITE(case_insensitive);

TEST(strcasecmp_equal) {
	ASSERT_EQ(0, strcasecmp("Hello", "HELLO"));
	ASSERT_EQ(0, strcasecmp("test", "TEST"));
	ASSERT_EQ(0, strcasecmp("MiXeD", "mixed"));
	ASSERT_EQ(0, strcasecmp("", ""));
	ASSERT_EQ(0, strcasecmp("ABC123", "abc123"));
}

TEST(strcasecmp_less_than) {
	ASSERT_TRUE(strcasecmp("abc", "xyz") < 0);
	ASSERT_TRUE(strcasecmp("Apple", "BANANA") < 0);
	ASSERT_TRUE(strcasecmp("test", "zebra") < 0);
}

TEST(strcasecmp_greater_than) {
	ASSERT_TRUE(strcasecmp("xyz", "abc") > 0);
	ASSERT_TRUE(strcasecmp("ZEBRA", "apple") > 0);
	ASSERT_TRUE(strcasecmp("banana", "Apple") > 0);
}

TEST(strcasecmp_empty_strings) {
	ASSERT_TRUE(strcasecmp("", "test") < 0);
	ASSERT_TRUE(strcasecmp("test", "") > 0);
}

TEST(strcasecmp_null_handling) {
	ASSERT_EQ(0, strcasecmp(NULL, NULL));
	ASSERT_TRUE(strcasecmp("test", NULL) > 0);
	ASSERT_TRUE(strcasecmp(NULL, "test") < 0);
}

TEST(strcasecmp_single_char) {
	ASSERT_EQ(0, strcasecmp("a", "A"));
	ASSERT_TRUE(strcasecmp("a", "B") < 0);
	ASSERT_TRUE(strcasecmp("Z", "a") > 0);
}

TEST(strcasecmp_with_numbers) {
	ASSERT_EQ(0, strcasecmp("test123", "TEST123"));
	ASSERT_TRUE(strcasecmp("1test", "2test") < 0);
}

TEST(strncasecmp_equal_within_n) {
	ASSERT_EQ(0, strncasecmp("Hello", "HELLO", 5));
	ASSERT_EQ(0, strncasecmp("test123", "TEST456", 4));
	ASSERT_EQ(0, strncasecmp("PREFIX_diff", "prefix_other", 6));
}

TEST(strncasecmp_different_within_n) {
	ASSERT_TRUE(strncasecmp("abcd", "AXCD", 2) < 0);
	ASSERT_TRUE(strncasecmp("ZEBRA", "apple", 1) > 0);
	ASSERT_TRUE(strncasecmp("test", "TEXT", 2) < 0);
}

TEST(strncasecmp_zero_length) {
	ASSERT_EQ(0, strncasecmp("anything", "different", 0));
	ASSERT_EQ(0, strncasecmp("", "", 0));
	ASSERT_EQ(0, strncasecmp("abc", "xyz", 0));
}

TEST(strncasecmp_null_handling) {
	ASSERT_EQ(0, strncasecmp(NULL, NULL, 5));
	ASSERT_TRUE(strncasecmp("test", NULL, 4) > 0);
	ASSERT_TRUE(strncasecmp(NULL, "test", 4) < 0);
	ASSERT_EQ(0, strncasecmp(NULL, NULL, 0));
}

TEST(strncasecmp_shorter_strings) {
	ASSERT_EQ(0, strncasecmp("Hi", "HI", 10));
	ASSERT_EQ(0, strncasecmp("short", "SHORT_LONG", 5));
	ASSERT_TRUE(strncasecmp("abc", "abcdef", 3) == 0);
}

TEST(strncasecmp_exact_length) {
	ASSERT_EQ(0, strncasecmp("test", "TEST", 4));
	ASSERT_TRUE(strncasecmp("test", "TESTX", 4) == 0);
}

/* ============================================================================
 * FIND FIRST SET (FFS) TESTS
 * ============================================================================ */
TEST_SUITE(find_first_set);

TEST(ffs_zero) {
	ASSERT_EQ(0, ffs(0));
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
	ASSERT_EQ(1, ffs(0x00000003));  // 0011 -> bit 1
	ASSERT_EQ(1, ffs(0x00000007));  // 0111 -> bit 1
	ASSERT_EQ(2, ffs(0x00000006));  // 0110 -> bit 2
	ASSERT_EQ(3, ffs(0x0000000C));  // 1100 -> bit 3
}

TEST(ffs_all_bits) {
	ASSERT_EQ(1, ffs(0xFFFFFFFF));  // All bits set -> first is 1
}

TEST(ffs_powers_of_two) {
	ASSERT_EQ(1, ffs(1 << 0));
	ASSERT_EQ(5, ffs(1 << 4));
	ASSERT_EQ(10, ffs(1 << 9));
	ASSERT_EQ(20, ffs(1 << 19));
}

TEST(ffsl_basic) {
	ASSERT_EQ(0, ffsl(0L));
	ASSERT_EQ(1, ffsl(1L));
	ASSERT_EQ(2, ffsl(2L));
	ASSERT_EQ(32, ffsl(0x80000000L));
}

#if JACL_64BIT
TEST(ffsl_64bit) {
	ASSERT_EQ(33, ffsl(0x100000000L));
	ASSERT_EQ(40, ffsl(0x8000000000L));
	ASSERT_EQ(64, ffsl(0x8000000000000000L));
}
#endif

TEST(ffsll_basic) {
	ASSERT_EQ(0, ffsll(0LL));
	ASSERT_EQ(1, ffsll(1LL));
	ASSERT_EQ(2, ffsll(2LL));
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
 * FIND LAST SET (FLS) TESTS
 * ============================================================================ */
TEST_SUITE(find_last_set);

TEST(fls_zero) {
	ASSERT_EQ(0, fls(0));
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
	ASSERT_EQ(2, fls(0x00000003));  // 0011 -> bit 2
	ASSERT_EQ(3, fls(0x00000007));  // 0111 -> bit 3
	ASSERT_EQ(3, fls(0x00000006));  // 0110 -> bit 3
	ASSERT_EQ(4, fls(0x0000000F));  // 1111 -> bit 4
}

TEST(fls_all_bits) {
	ASSERT_EQ(32, fls(0xFFFFFFFF));
}

TEST(fls_powers_of_two) {
	ASSERT_EQ(1, fls(1 << 0));
	ASSERT_EQ(5, fls(1 << 4));
	ASSERT_EQ(10, fls(1 << 9));
	ASSERT_EQ(20, fls(1 << 19));
}

TEST(flsl_basic) {
	ASSERT_EQ(0, flsl(0L));
	ASSERT_EQ(1, flsl(1L));
	ASSERT_EQ(2, flsl(2L));
	ASSERT_EQ(32, flsl(0x80000000L));
}

#if JACL_64BIT
TEST(flsl_64bit) {
	ASSERT_EQ(33, flsl(0x100000000L));
	ASSERT_EQ(40, flsl(0x8000000000L));
	ASSERT_EQ(64, flsl(0x8000000000000000L));
}
#endif

TEST(flsll_basic) {
	ASSERT_EQ(0, flsll(0LL));
	ASSERT_EQ(1, flsll(1LL));
	ASSERT_EQ(2, flsll(2LL));
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
 * LEGACY BSD MEMORY TESTS
 * ============================================================================ */
TEST_SUITE(legacy_memory);

TEST(bzero_basic) {
	char buffer[10] = "test1234";
	
	bzero(buffer, 4);
	
	ASSERT_EQ(0, buffer[0]);
	ASSERT_EQ(0, buffer[1]);
	ASSERT_EQ(0, buffer[2]);
	ASSERT_EQ(0, buffer[3]);
	ASSERT_EQ('1', buffer[4]);
}

TEST(bzero_full_clear) {
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

TEST(bcmp_equal) {
	char buf1[5] = "test";
	char buf2[5] = "test";
	
	ASSERT_EQ(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_different) {
	char buf1[5] = "test";
	char buf2[5] = "TEST";
	
	ASSERT_NE(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_partial_match) {
	char buf1[6] = "hello";
	char buf2[6] = "help!";
	
	ASSERT_EQ(0, bcmp(buf1, buf2, 3));
	ASSERT_NE(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_binary_data) {
	unsigned char a[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char b[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char c[4] = {0xFF, 0x00, 0xBB, 0x55};
	
	ASSERT_EQ(0, bcmp(a, b, 4));
	ASSERT_NE(0, bcmp(a, c, 4));
}

/* ============================================================================
 * LEGACY BSD STRING SEARCH TESTS
 * ============================================================================ */
TEST_SUITE(legacy_search);

TEST(index_found) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 4, index(str, 'o'));
	ASSERT_EQ(str + 0, index(str, 'h'));
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

TEST(index_first_occurrence) {
	const char *str = "banana";
	
	ASSERT_EQ(str + 1, index(str, 'a'));
}

TEST(rindex_found) {
	const char *str = "hello world";
	
	ASSERT_EQ(str + 7, rindex(str, 'o'));
	ASSERT_EQ(str + 9, rindex(str, 'l'));
	ASSERT_EQ(str + 0, rindex(str, 'h'));
}

TEST(rindex_not_found) {
	const char *str = "hello";
	
	ASSERT_NULL(rindex(str, 'z'));
	ASSERT_NULL(rindex(str, 'X'));
}

TEST(rindex_last_occurrence) {
	const char *str = "banana";
	
	ASSERT_EQ(str + 5, rindex(str, 'a'));
	ASSERT_EQ(str + 4, rindex(str, 'n'));
}

TEST(rindex_single_occurrence) {
	const char *str = "hello";
	
	ASSERT_EQ(str + 0, rindex(str, 'h'));
	ASSERT_EQ(str + 1, rindex(str, 'e'));
}

/* ============================================================================
 * BIT PATTERN TESTS
 * ============================================================================ */
TEST_SUITE(bit_patterns);

TEST(ffs_alternating_bits) {
	ASSERT_EQ(1, ffs(0xAAAAAAAA));  // 1010...
	ASSERT_EQ(2, ffs(0x55555555));  // 0101...
}

TEST(fls_alternating_bits) {
	ASSERT_EQ(32, fls(0xAAAAAAAA));
	ASSERT_EQ(31, fls(0x55555555));
}

TEST(ffs_sparse_bits) {
	ASSERT_EQ(5, ffs(0x00000010));
	ASSERT_EQ(9, ffs(0x00000100));
	ASSERT_EQ(17, ffs(0x00010000));
	ASSERT_EQ(25, ffs(0x01000000));
}

TEST(fls_sparse_bits) {
	ASSERT_EQ(5, fls(0x00000010));
	ASSERT_EQ(9, fls(0x00000100));
	ASSERT_EQ(17, fls(0x00010000));
	ASSERT_EQ(25, fls(0x01000000));
}

TEST(bit_boundaries) {
	ASSERT_EQ(1, ffs(0x00000001));
	ASSERT_EQ(32, ffs(0x80000000));
	ASSERT_EQ(1, fls(0x00000001));
	ASSERT_EQ(32, fls(0x80000000));
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(empty_string_comparisons) {
	ASSERT_EQ(0, strcasecmp("", ""));
	ASSERT_EQ(0, strncasecmp("", "", 0));
	ASSERT_EQ(0, strncasecmp("", "", 10));
}

TEST(single_character_operations) {
	ASSERT_EQ(0, strcasecmp("a", "A"));
	ASSERT_TRUE(strcasecmp("a", "b") < 0);
	
	const char *single = "x";
	ASSERT_EQ(single, index(single, 'x'));
	ASSERT_EQ(single, rindex(single, 'x'));
}

TEST(special_characters) {
	ASSERT_EQ(0, strcasecmp("@#$", "@#$"));
	ASSERT_TRUE(strcasecmp("!", "@") < 0);
}

TEST(unicode_handling) {
	// ASCII only - extended chars may not be case-folded
	ASSERT_EQ(0, strcasecmp("Test123!@#", "TEST123!@#"));
}

/* ============================================================================
 * STRESS TESTS
 * ============================================================================ */
TEST_SUITE(stress);

TEST(many_bit_operations) {
	for (int i = 0; i < 32; i++) {
		int mask = 1 << i;
		ASSERT_EQ(i + 1, ffs(mask));
		ASSERT_EQ(i + 1, fls(mask));
	}
}

TEST(long_string_comparisons) {
	char long1[1000];
	char long2[1000];
	
	memset(long1, 'a', 999);
	long1[999] = '\0';
	memset(long2, 'A', 999);
	long2[999] = '\0';
	
	ASSERT_EQ(0, strcasecmp(long1, long2));
	ASSERT_EQ(0, strncasecmp(long1, long2, 999));
}

TEST(repeated_operations) {
	for (int i = 0; i < 100; i++) {
		ASSERT_EQ(0, strcasecmp("test", "TEST"));
		ASSERT_EQ(5, ffs(0x00000010));
		ASSERT_EQ(5, fls(0x00000010));
	}
}

TEST_MAIN()
