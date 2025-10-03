/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <strings.h>

TEST_SUITE(strings);

/* ===== strcasecmp Tests ===== */
TEST(strcasecmp_equal_strings) {
		ASSERT_EQ(0, strcasecmp("Hello", "HELLO"));
		ASSERT_EQ(0, strcasecmp("test", "TEST"));
		ASSERT_EQ(0, strcasecmp("MiXeD", "mixed"));
		ASSERT_EQ(0, strcasecmp("", ""));
}

TEST(strcasecmp_different_strings) {
		ASSERT_TRUE(strcasecmp("abc", "xyz") < 0);
		ASSERT_TRUE(strcasecmp("xyz", "abc") > 0);
		ASSERT_TRUE(strcasecmp("Apple", "BANANA") < 0);
		ASSERT_TRUE(strcasecmp("ZEBRA", "apple") > 0);
}

TEST(strcasecmp_null_handling) {
		ASSERT_EQ(0, strcasecmp(NULL, NULL));
		ASSERT_TRUE(strcasecmp("test", NULL) > 0);
		ASSERT_TRUE(strcasecmp(NULL, "test") < 0);
}

TEST(strcasecmp_empty_strings) {
		ASSERT_TRUE(strcasecmp("", "test") < 0);
		ASSERT_TRUE(strcasecmp("test", "") > 0);
		ASSERT_EQ(0, strcasecmp("", ""));
}

/* ===== strncasecmp Tests ===== */
TEST(strncasecmp_equal_within_n) {
		ASSERT_EQ(0, strncasecmp("Hello", "HELLO", 5));
		ASSERT_EQ(0, strncasecmp("test123", "TEST456", 4));
		ASSERT_EQ(0, strncasecmp("PREFIX_diff", "prefix_other", 6));
}

TEST(strncasecmp_different_within_n) {
		ASSERT_TRUE(strncasecmp("abcd", "AXCD", 2) < 0);
		ASSERT_TRUE(strncasecmp("ZEBRA", "apple", 1) > 0);
}

TEST(strncasecmp_zero_length) {
		ASSERT_EQ(0, strncasecmp("anything", "different", 0));
		ASSERT_EQ(0, strncasecmp("", "", 0));
}

TEST(strncasecmp_null_handling) {
		ASSERT_EQ(0, strncasecmp(NULL, NULL, 5));
		ASSERT_TRUE(strncasecmp("test", NULL, 4) > 0);
		ASSERT_TRUE(strncasecmp(NULL, "test", 4) < 0);
}

TEST(strncasecmp_shorter_strings) {
		ASSERT_EQ(0, strncasecmp("Hi", "HI", 10));	// n > string length
		ASSERT_TRUE(strncasecmp("short", "SHORT_LONG", 5) == 0);
}

/* ===== ffs (Find First Set) Tests ===== */
TEST(ffs_basic_operations) {
		ASSERT_EQ(0, ffs(0));						// No bits set
		ASSERT_EQ(1, ffs(1));						// 0001 -> bit 1
		ASSERT_EQ(2, ffs(2));						// 0010 -> bit 2
		ASSERT_EQ(3, ffs(4));						// 0100 -> bit 3
		ASSERT_EQ(4, ffs(8));						// 1000 -> bit 4
}

TEST(ffs_multiple_bits) {
		ASSERT_EQ(1, ffs(3));						// 0011 -> first bit is 1
		ASSERT_EQ(1, ffs(7));						// 0111 -> first bit is 1
		ASSERT_EQ(2, ffs(6));						// 0110 -> first bit is 2
		ASSERT_EQ(1, ffs(0xFFFF));			// All bits -> first is 1
}

TEST(ffs_edge_cases) {
		ASSERT_EQ(32, ffs(0x80000000)); // MSB set (bit 32)
		ASSERT_EQ(1, ffs(0x80000001));	// LSB and MSB -> LSB first
		ASSERT_EQ(16, ffs(0x00008000)); // Bit 16 set
}

/* ===== ffsl and ffsll Tests ===== */
TEST(ffsl_operations) {
		ASSERT_EQ(0, ffsl(0L));
		ASSERT_EQ(1, ffsl(1L));
		ASSERT_EQ(8, ffsl(0x80L));
		ASSERT_EQ(1, ffsl(0xFFFFFFFFL));
}

TEST(ffsll_operations) {
		ASSERT_EQ(0, ffsll(0LL));
		ASSERT_EQ(1, ffsll(1LL));
		ASSERT_EQ(33, ffsll(0x100000000LL));	// Bit 33 set
		ASSERT_EQ(64, ffsll(0x8000000000000000LL)); // MSB of 64-bit
}

/* ===== fls (Find Last Set) Tests ===== */
TEST(fls_basic_operations) {
		ASSERT_EQ(0, fls(0));						// No bits set
		ASSERT_EQ(1, fls(1));						// 0001 -> bit 1
		ASSERT_EQ(2, fls(2));						// 0010 -> bit 2	
		ASSERT_EQ(2, fls(3));						// 0011 -> highest is bit 2
		ASSERT_EQ(3, fls(4));						// 0100 -> bit 3
}

TEST(fls_multiple_bits) {
		ASSERT_EQ(3, fls(7));						// 0111 -> highest is bit 3
		ASSERT_EQ(4, fls(15));					// 1111 -> highest is bit 4
		ASSERT_EQ(32, fls(0x80000000)); // MSB set
		ASSERT_EQ(16, fls(0x0000FFFF)); // Upper 16 bits
}

/* ===== flsl and flsll Tests ===== */
TEST(flsl_operations) {
		ASSERT_EQ(0, flsl(0L));
		ASSERT_EQ(1, flsl(1L));
		ASSERT_EQ(32, flsl(0x80000000L));
}

TEST(flsll_operations) {
		ASSERT_EQ(0, flsll(0LL));
		ASSERT_EQ(1, flsll(1LL));
		ASSERT_EQ(64, flsll(0x8000000000000000LL));
		ASSERT_EQ(33, flsll(0x100000000LL));
}

/* ===== bzero Tests ===== */
TEST(bzero_memory_clear) {
		char buffer[10] = "test1234";
		bzero(buffer, 4);
		ASSERT_EQ(0, buffer[0]);
		ASSERT_EQ(0, buffer[1]); 
		ASSERT_EQ(0, buffer[2]);
		ASSERT_EQ(0, buffer[3]);
		ASSERT_EQ('1', buffer[4]); // Should be unchanged
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
		ASSERT_EQ('t', buffer[0]); // Should be unchanged
}


/* ===== bcopy Tests ===== */
#ifndef __STRICT_ANSI__
TEST(bcopy_non_overlapping) {
		char src[6] = "hello";
		char dest[6] = {0};
		bcopy(src, dest, 5);
		ASSERT_EQ(0, strcmp(dest, "hello"));
}

TEST(bcopy_overlapping) {
		char buffer[10] = "abcdefgh";
		bcopy(buffer, buffer + 2, 4);  // Copy "abcd" to position 2
		ASSERT_EQ('a', buffer[2]);
		ASSERT_EQ('b', buffer[3]);
		ASSERT_EQ('c', buffer[4]);
		ASSERT_EQ('d', buffer[5]);
}
#endif

/* ===== bcmp Tests ===== */
#ifndef __STRICT_ANSI__
TEST(bcmp_equal_memory) {
		char buf1[5] = "test";
		char buf2[5] = "test";
		ASSERT_EQ(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_different_memory) {
		char buf1[5] = "test";
		char buf2[5] = "TEST";
		ASSERT_NE(0, bcmp(buf1, buf2, 4));
}

TEST(bcmp_partial_match) {
		char buf1[6] = "hello";
		char buf2[6] = "help!";
		ASSERT_EQ(0, bcmp(buf1, buf2, 3));	// "hel" matches
		ASSERT_NE(0, bcmp(buf1, buf2, 4));	// "hell" vs "help"
}
#endif

/* ===== index Tests ===== */
#ifndef __STRICT_ANSI__
TEST(index_find_character) {
		const char *str = "hello world";
		ASSERT_EQ(str + 4, index(str, 'o'));		 // First 'o'
		ASSERT_EQ(str + 0, index(str, 'h'));		 // First char
		ASSERT_EQ(str + 10, index(str, 'd'));		 // Last char
}

TEST(index_not_found) {
		const char *str = "hello";
		ASSERT_EQ(NULL, index(str, 'z'));
		ASSERT_EQ(NULL, index(str, 'X'));
}

TEST(index_null_terminator) {
		const char *str = "test";
		ASSERT_EQ(str + 4, index(str, '\0'));
}
#endif

/* ===== rindex Tests ===== */
#ifndef __STRICT_ANSI__
TEST(rindex_find_last_character) {
		const char *str = "hello world";
		ASSERT_EQ(str + 7, rindex(str, 'o'));		 // Last 'o'
		ASSERT_EQ(str + 2, rindex(str, 'l'));		 // Last 'l' in "hello"
		ASSERT_EQ(str + 0, rindex(str, 'h'));		 // Only 'h'
}

TEST(rindex_not_found) {
		const char *str = "hello";
		ASSERT_EQ(NULL, rindex(str, 'z'));
		ASSERT_EQ(NULL, rindex(str, 'X'));
}

TEST(rindex_multiple_occurrences) {
		const char *str = "banana";
		ASSERT_EQ(str + 5, rindex(str, 'a'));		 // Last 'a'
		ASSERT_EQ(str + 4, rindex(str, 'n'));		 // Last 'n'
}
#endif

/* ===== Edge Case and Stress Tests ===== */
TEST(stress_bit_patterns) {
		// Test various bit patterns
		ASSERT_EQ(1, ffs(0x00000001));
		ASSERT_EQ(17, ffs(0x00010000));
		ASSERT_EQ(32, fls(0x80000000));
		ASSERT_EQ(1, fls(0x00000001));
}

TEST(string_comparison_edge_cases) {
		// Single character differences
		ASSERT_TRUE(strcasecmp("a", "B") < 0);
		ASSERT_TRUE(strcasecmp("Z", "a") > 0);
		
		// Numbers vs letters
		ASSERT_TRUE(strcasecmp("1", "A") < 0);
		ASSERT_TRUE(strcasecmp("9", "A") < 0);
}

TEST_MAIN()

