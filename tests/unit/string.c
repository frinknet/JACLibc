/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>

TEST_TYPE(unit)
TEST_UNIT(string.h)

/* ============================================================================
 * memcpy() TESTS
 * ============================================================================ */
TEST_SUITE(memcpy)

TEST(memcpy_basic) {
	char src[] = "Hello World";
	char dest[20];
	void *result = memcpy(dest, src, 12);

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("Hello World", dest);
}

TEST(memcpy_partial) {
	char src[] = "Hello";
	char dest[10];
	memcpy(dest, src, 3);
	dest[3] = '\0';

	ASSERT_STR_EQ("Hel", dest);
}

TEST(memcpy_zero_bytes) {
	char src[] = "test";
	char dest[10] = "orig";
	memcpy(dest, src, 0);

	ASSERT_STR_EQ("orig", dest);
}

TEST(memcpy_binary_data) {
	unsigned char src[5] = {0x00, 0xFF, 0xAA, 0x55, 0x00};
	unsigned char dest[5];
	memcpy(dest, src, 5);

	ASSERT_EQ(0x00, dest[0]);
	ASSERT_EQ(0xFF, dest[1]);
	ASSERT_EQ(0xAA, dest[2]);
	ASSERT_EQ(0x55, dest[3]);
	ASSERT_EQ(0x00, dest[4]);
}

TEST(memcpy_null_pointers) {
	char buf[10];

	ASSERT_NULL(memcpy(NULL, "test", 4));
	ASSERT_NULL(memcpy(buf, NULL, 4));
	ASSERT_NULL(memcpy(NULL, NULL, 4));
}

/* ============================================================================
 * memmove() TESTS
 * ============================================================================ */
TEST_SUITE(memmove)

TEST(memmove_basic) {
	char src[] = "Hello";
	char dest[10];
	void *result = memmove(dest, src, 6);

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("Hello", dest);
}

TEST(memmove_overlap_forward) {
	char buffer[20] = "0123456789";
	memmove(buffer + 3, buffer, 7);

	ASSERT_EQ('0', buffer[3]);
	ASSERT_EQ('1', buffer[4]);
	ASSERT_EQ('6', buffer[9]);
}

TEST(memmove_overlap_backward) {
	char buffer[20] = "0123456789";
	memmove(buffer, buffer + 3, 7);

	ASSERT_EQ('3', buffer[0]);
	ASSERT_EQ('4', buffer[1]);
	ASSERT_EQ('9', buffer[6]);
}

TEST(memmove_same_location) {
	char buffer[10] = "test";
	memmove(buffer, buffer, 5);

	ASSERT_STR_EQ("test", buffer);
}

TEST(memmove_null_pointers) {
	char buf[10];

	ASSERT_NULL(memmove(NULL, "test", 4));
	ASSERT_NULL(memmove(buf, NULL, 4));
}

/* ============================================================================
 * memset() TESTS
 * ============================================================================ */
TEST_SUITE(memset)

TEST(memset_basic) {
	char buffer[10];
	void *result = memset(buffer, 'A', 5);
	buffer[5] = '\0';

	ASSERT_EQ(buffer, result);
	ASSERT_STR_EQ("AAAAA", buffer);
}

TEST(memset_zero_fill) {
	char buffer[10] = "test";
	memset(buffer, 0, 10);

	for (int i = 0; i < 10; i++) {
		ASSERT_EQ(0, buffer[i]);
	}
}

TEST(memset_full_range) {
	unsigned char buffer[5];
	memset(buffer, 0xFF, 5);

	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(0xFF, buffer[i]);
	}
}

TEST(memset_null_pointer) {
	ASSERT_NULL(memset(NULL, 'A', 10));
}

/* ============================================================================
 * memcmp() TESTS
 * ============================================================================ */
TEST_SUITE(memcmp)

TEST(memcmp_equal) {
	ASSERT_EQ(0, memcmp("hello", "hello", 5));
	ASSERT_EQ(0, memcmp("test", "test", 4));
}

TEST(memcmp_less_than) {
	ASSERT_TRUE(memcmp("abc", "abd", 3) < 0);
	ASSERT_TRUE(memcmp("aaa", "aab", 3) < 0);
}

TEST(memcmp_greater_than) {
	ASSERT_TRUE(memcmp("abd", "abc", 3) > 0);
	ASSERT_TRUE(memcmp("aab", "aaa", 3) > 0);
}

TEST(memcmp_zero_length) {
	ASSERT_EQ(0, memcmp("different", "strings", 0));
}

TEST(memcmp_binary_data) {
	unsigned char a[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char b[4] = {0xFF, 0x00, 0xAA, 0x55};
	unsigned char c[4] = {0xFF, 0x00, 0xBB, 0x55};

	ASSERT_EQ(0, memcmp(a, b, 4));
	ASSERT_TRUE(memcmp(a, c, 4) < 0);
}

TEST(memcmp_embedded_nulls) {
	char a[5] = {'a', '\0', 'b', 'c', 'd'};
	char b[5] = {'a', '\0', 'b', 'c', 'e'};

	ASSERT_TRUE(memcmp(a, b, 5) < 0);
}

/* ============================================================================
 * memchr() TESTS
 * ============================================================================ */
TEST_SUITE(memchr)

TEST(memchr_found) {
	const char *str = "hello world";

	ASSERT_EQ(str + 4, memchr(str, 'o', 11));
	ASSERT_EQ(str + 0, memchr(str, 'h', 11));
	ASSERT_EQ(str + 10, memchr(str, 'd', 11));
}

TEST(memchr_not_found) {
	const char *str = "hello";

	ASSERT_NULL(memchr(str, 'x', 5));
	ASSERT_NULL(memchr(str, 'o', 3));
}

TEST(memchr_null_byte) {
	const char *str = "hello\0world";

	ASSERT_EQ(str + 5, memchr(str, '\0', 11));
}

TEST(memchr_binary_search) {
	unsigned char data[5] = {0x00, 0xFF, 0xAA, 0x55, 0x00};

	ASSERT_EQ(data + 1, memchr(data, 0xFF, 5));
	ASSERT_EQ(data + 2, memchr(data, 0xAA, 5));
}

/* ============================================================================
 * memmem() TESTS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(memmem)

TEST(memmem_found) {
	const char *haystack = "hello world hello";

	ASSERT_EQ(haystack + 0, memmem(haystack, 17, "hello", 5));
	ASSERT_EQ(haystack + 6, memmem(haystack, 17, "world", 5));
}

TEST(memmem_not_found) {
	const char *haystack = "hello world";

	ASSERT_NULL(memmem(haystack, 11, "goodbye", 7));
}

TEST(memmem_empty_needle) {
	const char *haystack = "test";

	ASSERT_EQ(haystack, memmem(haystack, 4, "", 0));
}

TEST(memmem_needle_larger) {
	ASSERT_NULL(memmem("hi", 2, "hello", 5));
}
#endif

/* ============================================================================
 * strlen() TESTS
 * ============================================================================ */
TEST_SUITE(strlen)

TEST(strlen_basic) {
	ASSERT_EQ(0, strlen(""));
	ASSERT_EQ(5, strlen("hello"));
	ASSERT_EQ(11, strlen("hello world"));
}

TEST(strlen_embedded_null) {
	char str[10] = {'a', 'b', '\0', 'c', 'd'};

	ASSERT_EQ(2, strlen(str));
}

/* ============================================================================
 * strnlen() TESTS
 * ============================================================================ */
TEST_SUITE(strnlen)

TEST(strnlen_within_limit) {
	ASSERT_EQ(5, strnlen("hello", 10));
	ASSERT_EQ(0, strnlen("", 10));
}

TEST(strnlen_exceeds_limit) {
	ASSERT_EQ(3, strnlen("hello", 3));
}

TEST(strnlen_zero_limit) {
	ASSERT_EQ(0, strnlen("test", 0));
}

/* ============================================================================
 * strcpy() TESTS
 * ============================================================================ */
TEST_SUITE(strcpy)

TEST(strcpy_basic) {
	char dest[20];
	char *result = strcpy(dest, "hello");

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("hello", dest);
}

TEST(strcpy_empty) {
	char dest[10];
	strcpy(dest, "");

	ASSERT_STR_EQ("", dest);
}

/* ============================================================================
 * strncpy() TESTS
 * ============================================================================ */
TEST_SUITE(strncpy)

TEST(strncpy_basic) {
	char dest[10];
	char *result = strncpy(dest, "hello", 10);

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("hello", dest);
}

TEST(strncpy_truncate) {
	char dest[10];
	memset(dest, 'X', 10);
	strncpy(dest, "hello", 3);
	dest[3] = '\0';

	ASSERT_STR_EQ("hel", dest);
}

TEST(strncpy_pad_nulls) {
	char dest[10];
	memset(dest, 'X', 10);
	strncpy(dest, "hi", 5);

	ASSERT_EQ('h', dest[0]);
	ASSERT_EQ('i', dest[1]);
	ASSERT_EQ('\0', dest[2]);
	ASSERT_EQ('\0', dest[3]);
	ASSERT_EQ('\0', dest[4]);
}

TEST(strncpy_no_null_termination) {
	char dest[10];
	memset(dest, 'X', 10);
	strncpy(dest, "12345", 5);

	ASSERT_EQ('X', dest[5]);
}

/* ============================================================================
 * strcat() TESTS
 * ============================================================================ */
TEST_SUITE(strcat)

TEST(strcat_basic) {
	char dest[20] = "hello";
	char *result = strcat(dest, " world");

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("hello world", dest);
}

TEST(strcat_empty_dest) {
	char dest[20] = "";
	strcat(dest, "test");

	ASSERT_STR_EQ("test", dest);
}

TEST(strcat_empty_src) {
	char dest[20] = "test";
	strcat(dest, "");

	ASSERT_STR_EQ("test", dest);
}

/* ============================================================================
 * strncat() TESTS
 * ============================================================================ */
TEST_SUITE(strncat)

TEST(strncat_basic) {
	char dest[20] = "hello";
	char *result = strncat(dest, " world!", 6);

	ASSERT_EQ(dest, result);
	ASSERT_STR_EQ("hello world", dest);
}

TEST(strncat_partial) {
	char dest[20] = "test";
	strncat(dest, "12345", 3);

	ASSERT_STR_EQ("test123", dest);
}

TEST(strncat_exact) {
	char dest[20] = "a";
	strncat(dest, "bcd", 3);

	ASSERT_STR_EQ("abcd", dest);
}

/* ============================================================================
 * strcmp() TESTS
 * ============================================================================ */
TEST_SUITE(strcmp)

TEST(strcmp_equal) {
	ASSERT_EQ(0, strcmp("hello", "hello"));
	ASSERT_EQ(0, strcmp("", ""));
}

TEST(strcmp_less_than) {
	ASSERT_TRUE(strcmp("abc", "abd") < 0);
	ASSERT_TRUE(strcmp("", "a") < 0);
}

TEST(strcmp_greater_than) {
	ASSERT_TRUE(strcmp("abd", "abc") > 0);
	ASSERT_TRUE(strcmp("a", "") > 0);
}

TEST(strcmp_different_lengths) {
	ASSERT_TRUE(strcmp("abc", "abcd") < 0);
	ASSERT_TRUE(strcmp("abcd", "abc") > 0);
}

/* ============================================================================
 * strncmp() TESTS
 * ============================================================================ */
TEST_SUITE(strncmp)

TEST(strncmp_equal_within_n) {
	ASSERT_EQ(0, strncmp("hello", "hello", 5));
	ASSERT_EQ(0, strncmp("hello123", "hello456", 5));
}

TEST(strncmp_different_within_n) {
	ASSERT_TRUE(strncmp("abc", "abd", 3) < 0);
	ASSERT_TRUE(strncmp("abd", "abc", 3) > 0);
}

TEST(strncmp_zero_length) {
	ASSERT_EQ(0, strncmp("different", "strings", 0));
}

/* ============================================================================
 * strcoll() TESTS
 * ============================================================================ */
TEST_SUITE(strcoll)

TEST(strcoll_basic) {
	ASSERT_EQ(0, strcoll("hello", "hello"));
	ASSERT_TRUE(strcoll("abc", "abd") < 0);
}

/* ============================================================================
 * strxfrm() TESTS
 * ============================================================================ */
TEST_SUITE(strxfrm)

TEST(strxfrm_basic) {
	char dest[20];
	size_t result = strxfrm(dest, "hello", 20);

	ASSERT_EQ(5, result);
	ASSERT_STR_EQ("hello", dest);
}

TEST(strxfrm_small_buffer) {
	char dest[3];
	size_t result = strxfrm(dest, "hello", 3);

	ASSERT_EQ(5, result);
}

/* ============================================================================
 * strerror() TESTS
 * ============================================================================ */
TEST_SUITE(strerror)

TEST(strerror_success) {
	char *msg = strerror(0);

	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Success", msg);
}

TEST(strerror_common_errors) {
	ASSERT_NOT_NULL(strerror(EPERM));
	ASSERT_NOT_NULL(strerror(ENOENT));
	ASSERT_NOT_NULL(strerror(ENOMEM));
	ASSERT_NOT_NULL(strerror(EACCES));
}

TEST(strerror_unknown) {
	char *msg = strerror(999);

	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Unknown error", msg);
}

/* ============================================================================
 * strerror_r() TESTS
 * ============================================================================ */
TEST_SUITE(strerror_r)

TEST(strerror_r_success) {
	char buffer[64];
	int result = strerror_r(0, buffer, 64);

	ASSERT_EQ(0, result);
	ASSERT_STR_EQ("Success", buffer);
}

TEST(strerror_r_truncation) {
	char buffer[5];
	int result = strerror_r(0, buffer, 5);

	ASSERT_EQ(ERANGE, result);
	ASSERT_EQ(4, strlen(buffer));
}

TEST(strerror_r_invalid) {
	char buf[10];

	ASSERT_EQ(EINVAL, strerror_r(0, NULL, 10));
	ASSERT_EQ(EINVAL, strerror_r(0, buf, 0));
}

/* ============================================================================
 * strchr() TESTS
 * ============================================================================ */
TEST_SUITE(strchr)

TEST(strchr_found) {
	const char *str = "hello world";

	ASSERT_EQ(str + 4, strchr(str, 'o'));
	ASSERT_EQ(str + 0, strchr(str, 'h'));
	ASSERT_EQ(str + 10, strchr(str, 'd'));
}

TEST(strchr_not_found) {
	const char *str = "hello";

	ASSERT_NULL(strchr(str, 'x'));
}

TEST(strchr_null_terminator) {
	const char *str = "hello";

	ASSERT_EQ(str + 5, strchr(str, '\0'));
}

/* ============================================================================
 * strrchr() TESTS
 * ============================================================================ */
TEST_SUITE(strrchr)

TEST(strrchr_last_occurrence) {
	const char *str = "hello world";

	ASSERT_EQ(str + 7, strrchr(str, 'o'));
	ASSERT_EQ(str + 9, strrchr(str, 'l'));
	ASSERT_EQ(str + 0, strrchr(str, 'h'));
}

TEST(strrchr_not_found) {
	ASSERT_NULL(strrchr("hello", 'x'));
}

/* ============================================================================
 * strstr() TESTS
 * ============================================================================ */
TEST_SUITE(strstr)

TEST(strstr_found) {
	const char *str = "hello world hello";

	ASSERT_EQ(str + 0, strstr(str, "hello"));
	ASSERT_EQ(str + 6, strstr(str, "world"));
	ASSERT_EQ(str + 12, strstr(str + 6, "hello"));
}

TEST(strstr_not_found) {
	ASSERT_NULL(strstr("hello", "goodbye"));
}

TEST(strstr_empty_needle) {
	const char *str = "test";

	ASSERT_EQ(str, strstr(str, ""));
}

/* ============================================================================
 * strspn() TESTS
 * ============================================================================ */
TEST_SUITE(strspn)

TEST(strspn_basic) {
	ASSERT_EQ(0, strspn("hello", "xyz"));
	ASSERT_EQ(5, strspn("hello", "helo"));
	ASSERT_EQ(3, strspn("abcdef", "abc"));
}

TEST(strspn_empty) {
	ASSERT_EQ(0, strspn("hello", ""));
	ASSERT_EQ(0, strspn("", "abc"));
}

/* ============================================================================
 * strcspn() TESTS
 * ============================================================================ */
TEST_SUITE(strcspn)

TEST(strcspn_basic) {
	ASSERT_EQ(5, strcspn("hello", "xyz"));
	ASSERT_EQ(2, strcspn("hello", "l"));
	ASSERT_EQ(0, strcspn("hello", "h"));
	ASSERT_EQ(3, strcspn("abcdef", "def"));
}

TEST(strcspn_empty) {
	ASSERT_EQ(5, strcspn("hello", ""));
}

/* ============================================================================
 * strpbrk() TESTS
 * ============================================================================ */
TEST_SUITE(strpbrk)

TEST(strpbrk_found) {
	const char *str = "hello world";

	ASSERT_EQ(str + 2, strpbrk(str, "l"));
	ASSERT_EQ(str + 2, strpbrk(str, "xl"));
	ASSERT_EQ(str + 4, strpbrk(str, "or"));
}

TEST(strpbrk_not_found) {
	ASSERT_NULL(strpbrk("hello", "xyz"));
}

/* ============================================================================
 * strtok() TESTS
 * ============================================================================ */
TEST_SUITE(strtok)

TEST(strtok_basic) {
	char str[] = "hello,world;test:end";
	char *tok;

	tok = strtok(str, ",;:");
	ASSERT_STR_EQ("hello", tok);

	tok = strtok(NULL, ",;:");
	ASSERT_STR_EQ("world", tok);

	tok = strtok(NULL, ",;:");
	ASSERT_STR_EQ("test", tok);

	tok = strtok(NULL, ",;:");
	ASSERT_STR_EQ("end", tok);

	tok = strtok(NULL, ",;:");
	ASSERT_NULL(tok);
}

TEST(strtok_consecutive_delimiters) {
	char str[] = "a,,b,,c";
	char *tok;

	tok = strtok(str, ",");
	ASSERT_STR_EQ("a", tok);

	tok = strtok(NULL, ",");
	ASSERT_STR_EQ("b", tok);

	tok = strtok(NULL, ",");
	ASSERT_STR_EQ("c", tok);
}

TEST(strtok_leading_trailing) {
	char str[] = ",,,abc,,,";
	char *tok;

	tok = strtok(str, ",");
	ASSERT_STR_EQ("abc", tok);

	tok = strtok(NULL, ",");
	ASSERT_NULL(tok);
}

TEST(strtok_empty_string) {
	char str[] = "";

	ASSERT_NULL(strtok(str, ","));
}

TEST(strtok_only_delimiters) {
	char str[] = ",,,";

	ASSERT_NULL(strtok(str, ","));
}

/* ============================================================================
 * strtok_r() TESTS
 * ============================================================================ */
TEST_SUITE(strtok_r)

TEST(strtok_r_basic) {
	char str1[] = "a,b,c";
	char str2[] = "x:y:z";
	char *save1, *save2;
	char *t1, *t2;

	t1 = strtok_r(str1, ",", &save1);
	t2 = strtok_r(str2, ":", &save2);
	ASSERT_STR_EQ("a", t1);
	ASSERT_STR_EQ("x", t2);

	t1 = strtok_r(NULL, ",", &save1);
	t2 = strtok_r(NULL, ":", &save2);
	ASSERT_STR_EQ("b", t1);
	ASSERT_STR_EQ("y", t2);
}

/* ============================================================================
 * strlcpy() TESTS (BSD)
 * ============================================================================ */
#if JACL_OS_BSD
TEST_SUITE(strlcpy)

TEST(strlcpy_basic) {
	char dest[10];
	size_t result = strlcpy(dest, "hello", 10);

	ASSERT_EQ(5, result);
	ASSERT_STR_EQ("hello", dest);
}

TEST(strlcpy_truncate) {
	char dest[5];
	size_t result = strlcpy(dest, "very long string", 5);

	ASSERT_EQ(16, result);
	ASSERT_STR_EQ("very", dest);
	ASSERT_EQ('\0', dest[4]);
}

/* ============================================================================
 * strlcat() TESTS (BSD)
 * ============================================================================ */
TEST_SUITE(strlcat)

TEST(strlcat_basic) {
	char dest[20] = "hello";
	size_t result = strlcat(dest, " world", 20);

	ASSERT_EQ(11, result);
	ASSERT_STR_EQ("hello world", dest);
}

TEST(strlcat_truncate) {
	char dest[10] = "hello";
	size_t result = strlcat(dest, " world!", 10);

	ASSERT_EQ(12, result);
	ASSERT_STR_EQ("hello wor", dest);
	ASSERT_EQ('\0', dest[9]);
}

TEST(strlcat_dest_full) {
	char dest[6] = "hello";
	size_t result = strlcat(dest, " world", 5);

	ASSERT_EQ(11, result);
	ASSERT_STR_EQ("hello", dest);
}
#endif


/* ============================================================================
 * basename() TESTS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(basename)

TEST(basename_basic) {
	ASSERT_STR_EQ("test", basename("/usr/bin/test"));
	ASSERT_STR_EQ("filename", basename("filename"));
	ASSERT_STR_EQ("", basename("/usr/bin/"));
	ASSERT_STR_EQ("", basename(""));
}
#endif

TEST_MAIN()

