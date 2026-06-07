/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>

TEST_TYPE(unit)
TEST_UNIT(string.h)

/* ============================================================================ */

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

/* ============================================================================ */

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

/* ============================================================================ */

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

/* ============================================================================ */

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

TEST(memcmp_null_a) {
	ASSERT_NE(0, memcmp(NULL, "a", 1));
}

TEST(memcmp_null_b) {
	ASSERT_NE(0, memcmp("a", NULL, 1));
}

TEST(memcmp_null_both) {
	ASSERT_EQ(0, memcmp(NULL, NULL, 0));
}

/* ============================================================================ */

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

TEST(memchr_null_s) {
	ASSERT_NULL(memchr(NULL, 'a', 1));
}

TEST(memchr_zero_n) {
	ASSERT_NULL(memchr("a", 'a', 0));
}

/* ============================================================================ */

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

TEST(memmem_null_haystack) {
	ASSERT_NULL(memmem(NULL, 1, "a", 1));
}

TEST(memmem_null_needle) {
	ASSERT_NULL(memmem("a", 1, NULL, 1));
}

/* ============================================================================ */

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

TEST(strlen_null) {
	ASSERT_EQ(0, strlen(NULL));
}

/* ============================================================================ */

TEST_SUITE(strnlen)

TEST(strnlen_within_limit) {
	ASSERT_EQ(5, strnlen("hello", 10));
	ASSERT_EQ(0, strnlen("", 10));
}

TEST(strnlen_null) {
	ASSERT_EQ(0, strnlen(NULL, 10));
}

TEST(strnlen_exceeds_limit) {
	ASSERT_EQ(3, strnlen("hello", 3));
}

TEST(strnlen_zero_limit) {
	ASSERT_EQ(0, strnlen("test", 0));
}

/* ============================================================================ */

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

TEST(strcpy_null_dest) {
	ASSERT_NULL(strcpy(NULL, "a"));
}

TEST(strcpy_null_src) {
	char d[2];
	ASSERT_NULL(strcpy(d, NULL));
}

/* ============================================================================ */

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

TEST(strncpy_null_dest) {
	ASSERT_NULL(strncpy(NULL, "a", 1));
}

TEST(strncpy_null_src) {
	char d[2];
	ASSERT_NULL(strncpy(d, NULL, 1));
}

TEST(strncpy_exact_fit) {
	char dest[6];
	memset(dest, 'X', 6);
	strncpy(dest, "hello", 5);
	ASSERT_EQ('o', dest[4]);
	ASSERT_EQ('X', dest[5]); // Not null terminated because n == strlen(src)
}

/* ============================================================================ */

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

TEST(strcat_null_dest) {
	ASSERT_NULL(strcat(NULL, "a"));
}

TEST(strcat_null_src) {
	char d[4] = "a";
	ASSERT_NULL(strcat(d, NULL));
}

/* ============================================================================ */

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

TEST(strncat_null_dest) {
	ASSERT_NULL(strncat(NULL, "a", 1));
}
TEST(strncat_null_src) {
	char d[4] = "a";
	ASSERT_NULL(strncat(d, NULL, 1));
}

/* ============================================================================ */

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

TEST(strcmp_null_a) {
	ASSERT_NE(0, strcmp(NULL, "a"));
}

TEST(strcmp_null_b) {
	ASSERT_NE(0, strcmp("a", NULL));
}

/* ============================================================================ */

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

TEST(strncmp_null_a) {
	ASSERT_NE(0, strncmp(NULL, "a", 1));
}

TEST(strncmp_null_b) {
	ASSERT_NE(0, strncmp("a", NULL, 1));
}

/* ============================================================================ */

TEST_SUITE(strcoll)

TEST(strcoll_basic) {
	ASSERT_EQ(0, strcoll("hello", "hello"));
	ASSERT_TRUE(strcoll("abc", "abd") < 0);
}

TEST(strcoll_null_a) {
	ASSERT_NE(0, strcoll(NULL, "a"));
}

TEST(strcoll_null_b) {
	ASSERT_NE(0, strcoll("a", NULL));
}

/* ============================================================================ */

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

TEST(strxfrm_null_dest) {
	ASSERT_EQ(1, strxfrm(NULL, "a", 10));
	ASSERT_EQ(0, strxfrm(NULL, "", 10));
}

TEST(strxfrm_null_src) {
	char d[2];
	ASSERT_EQ(0, strxfrm(d, NULL, 10));
}

/* ============================================================================ */

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

/* ============================================================================ */

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

TEST(strerror_r_exact_fit) {
	char buf[8]; // "Success" is 7 chars + 1 null = 8 bytes
	int ret = strerror_r(0, buf, 8);
	ASSERT_EQ(0, ret);
	ASSERT_STR_EQ("Success", buf);
}

TEST(strerror_r_off_by_one) {
	char buf[7]; // Needs 8, has 7
	int ret = strerror_r(0, buf, 7);
	ASSERT_EQ(ERANGE, ret);
	ASSERT_EQ(6, strlen(buf)); // Truncated to "Succe" + '\0'
}

/* ============================================================================ */

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

TEST(strchr_null_s) {
	ASSERT_NULL(strchr(NULL, 'a'));
}

/* ============================================================================ */

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

TEST(strrchr_null_s) {
	ASSERT_NULL(strrchr(NULL, 'a'));
}

TEST(strrchr_null_terminator) {
	const char *str = "hello";
	ASSERT_EQ(str + 5, strrchr(str, '\0'));
}

/* ============================================================================ */

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

TEST(strstr_null_haystack) {
	ASSERT_NULL(strstr(NULL, "a"));
}

TEST(strstr_null_needle) {
	ASSERT_NULL(strstr("a", NULL));
}

/* ============================================================================ */

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

TEST(strspn_null_s) {
	ASSERT_EQ(0, strspn(NULL, "a"));
}

TEST(strspn_null_accept) {
	ASSERT_EQ(0, strspn("a", NULL));
}

TEST(strspn_all_match) {
	ASSERT_EQ(5, strspn("hello", "helo"));
}

/* ============================================================================ */

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

TEST(strcspn_null_s) {
	ASSERT_EQ(0, strcspn(NULL, "a"));
}

TEST(strcspn_null_reject) {
	ASSERT_EQ(1, strcspn("a", NULL));
}

TEST(strcspn_no_match) {
	ASSERT_EQ(5, strcspn("hello", "xyz"));
}

/* ============================================================================ */

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

TEST(strpbrk_null_s) {
	ASSERT_NULL(strpbrk(NULL, "a"));
}

TEST(strpbrk_null_accept) {
	ASSERT_NULL(strpbrk("a", NULL));
}

/* ============================================================================ */

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

/* ============================================================================ */

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

TEST(strtok_r_null_delim) {
	char *save;
	ASSERT_NULL(strtok_r("a", NULL, &save));
}

TEST(strtok_r_null_save) {
	ASSERT_NULL(strtok_r("a", ",", NULL));
}

TEST(strtok_r_empty_delim) {
	char str[] = "hello";
	char *save;
	char *tok = strtok_r(str, "", &save);
	ASSERT_STR_EQ("hello", tok);
	ASSERT_NULL(strtok_r(NULL, "", &save));
}

/* ============================================================================ */

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

TEST(strlcpy_zero_n) {
	char dest[5] = "XXXX";
	size_t ret = strlcpy(dest, "hello", 0);
	ASSERT_EQ(5, ret);
	ASSERT_EQ('X', dest[0]); // Buffer untouched
}

TEST(strlcpy_null_dest) {
	ASSERT_EQ(1, strlcpy(NULL, "a", 10));
}

TEST(strlcpy_null_src) {
	char d[2];
	ASSERT_EQ(0, strlcpy(d, NULL, 10));
}

TEST(strlcpy_n_is_1) {
	char dest[2];
	memset(dest, 'X', 2); // Properly initialize both bytes to 'X'
	size_t ret = strlcpy(dest, "hello", 1);
	ASSERT_EQ(5, ret);
	ASSERT_EQ('\0', dest[0]); // Only the null terminator is written
	ASSERT_EQ('X', dest[1]);  // Rest of buffer untouched
}

/* ============================================================================ */

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

TEST(strlcat_zero_n) {
	char dest[5] = "XXXX";
	size_t ret = strlcat(dest, "hello", 0);
	ASSERT_EQ(5, ret);
	ASSERT_EQ('X', dest[0]); // Buffer untouched
}

TEST(strlcat_null_dest) {
	ASSERT_EQ(1, strlcat(NULL, "a", 10));
}

TEST(strlcat_null_src) {
	char d[4] = "a";
	ASSERT_EQ(1, strlcat(d, NULL, 10));
}

TEST(strlcat_empty_dest_string) {
	char d[10] = "";
	ASSERT_EQ(5, strlcat(d, "hello", 10));
	ASSERT_STR_EQ("hello", d);
}

TEST(strlcat_n_is_1) {
	char dest[5] = "a";
	size_t ret = strlcat(dest, "hello", 1);
	ASSERT_EQ(6, ret); // 1 (dlen) + 5 (slen)
	ASSERT_STR_EQ("a", dest); // Buffer untouched because n <= dlen
}

/* ============================================================================ */

TEST_SUITE(memccpy)

TEST(memccpy_found) {
	char dest[10];
	void *ret = memccpy(dest, "hello", 'l', 5);
	ASSERT_EQ(dest + 3, ret);
	ASSERT_EQ('h', dest[0]);
	ASSERT_EQ('e', dest[1]);
	ASSERT_EQ('l', dest[2]);
}

TEST(memccpy_not_found) {
	char dest[10];
	void *ret = memccpy(dest, "hello", 'x', 5);
	ASSERT_NULL(ret);
	ASSERT_EQ('o', dest[4]);
}

TEST(memccpy_null_dest) {
	ASSERT_NULL(memccpy(NULL, "a", 'a', 1));
}

TEST(memccpy_null_src) {
	char d[2];
	ASSERT_NULL(memccpy(d, NULL, 'a', 1));
}

TEST(memccpy_exact_match) {
	char d[10];
	void *ret = memccpy(d, "abc", 'c', 3);
	ASSERT_EQ(d + 3, ret);
	ASSERT_EQ('c', d[2]);
}

/* ============================================================================ */

TEST_SUITE(stpcpy)

TEST(stpcpy_basic) {
	char dest[10];
	char *ret = stpcpy(dest, "test");
	ASSERT_EQ(dest + 4, ret);
	ASSERT_EQ('\0', *ret);
	ASSERT_STR_EQ("test", dest);
}

TEST(stpcpy_null_dest) {
	ASSERT_NULL(stpcpy(NULL, "a"));
}

TEST(stpcpy_null_src) {
	char d[2];
	ASSERT_NULL(stpcpy(d, NULL));
}

TEST(stpcpy_empty) {
	char dest[10] = "X";
	char *ret = stpcpy(dest, "");
	ASSERT_EQ(dest, ret);
	ASSERT_EQ('\0', *ret);
}

/* ============================================================================ */

TEST_SUITE(stpncpy)

TEST(stpncpy_basic) {
	char dest[10] = {0};
	char *ret = stpncpy(dest, "hello", 3);
	ASSERT_EQ(dest + 3, ret);
	ASSERT_EQ('h', dest[0]);
	ASSERT_EQ('l', dest[2]);
}

TEST(stpncpy_pads) {
	char dest[10];
	memset(dest, 'X', 10);
	char *ret = stpncpy(dest, "hi", 5);
	ASSERT_EQ(dest + 2, ret);
	ASSERT_EQ('\0', dest[2]);
	ASSERT_EQ('\0', dest[4]);
}

TEST(stpncpy_null_dest) {
	ASSERT_NULL(stpncpy(NULL, "a", 1));
}

TEST(stpncpy_null_src) {
	char d[2];
	ASSERT_NULL(stpncpy(d, NULL, 1));
}

TEST(stpncpy_exact_no_pad) {
	char dest[5];
	memset(dest, 'X', 5);
	char *ret = stpncpy(dest, "hello", 5);
	ASSERT_EQ(dest + 5, ret);
	ASSERT_EQ('o', dest[4]); // POSIX: If src length == n, NO null terminator is added
}

/* ============================================================================ */

TEST_SUITE(strdup)

TEST(strdup_basic) {
	char *dup = strdup("test");
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("test", dup);
	free(dup);
}

TEST(strdup_null) {
	ASSERT_NULL(strdup(NULL));
}

TEST(strdup_empty) {
	char *dup = strdup("");
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("", dup);
	free(dup);
}

TEST(strdup_stops_at_null) {
	char src[] = {'a', 'b', '\0', 'c', 'd', '\0'};
	char *dup = strdup(src);
	ASSERT_NOT_NULL(dup);
	ASSERT_EQ(2, strlen(dup));
	ASSERT_STR_EQ("ab", dup);
	free(dup);
}

/* ============================================================================ */

TEST_SUITE(strndup)

TEST(strndup_basic) {
	char *dup = strndup("hello world", 5);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("hello", dup);
	free(dup);
}

TEST(strndup_short) {
	char *dup = strndup("hi", 10);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("hi", dup);
	free(dup);
}

TEST(strndup_null) {
	ASSERT_NULL(strndup(NULL, 10));
}

TEST(strndup_zero_n) {
	char *dup = strndup("hello", 0);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("", dup);
	free(dup);
}

TEST(strndup_exact_length) {
	char *dup = strndup("hello", 5);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("hello", dup);
	ASSERT_EQ('\0', dup[5]);
	free(dup);
}

TEST(strndup_no_null_in_n) {
	char src[] = {'a', 'b', 'c', 'd', 'e'}; // No null terminator in source array
	char *dup = strndup(src, 5);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("abcde", dup);
	ASSERT_EQ('\0', dup[5]); // strndup MUST add the null terminator
	free(dup);
}

TEST(strndup_large_n) {
	char *dup = strndup("hi", 100);
	ASSERT_NOT_NULL(dup);
	ASSERT_STR_EQ("hi", dup);
	free(dup);
}

/* ============================================================================ */

TEST_SUITE(strsignal)

TEST(strsignal_basic) {
	char *msg = strsignal(SIGSEGV);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Segmentation fault", msg);
}

TEST(strsignal_unknown) {
	char *msg = strsignal(9999);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Unknown signal", msg);
}

TEST(strsignal_sigint) {
	char *msg = strsignal(SIGINT);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Interrupt", msg);
}

TEST(strsignal_sigkill) {
	char *msg = strsignal(SIGKILL);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Killed", msg);
}

TEST(strsignal_zero) {
	char *msg = strsignal(0);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Unknown signal", msg);
}

TEST(strsignal_negative) {
	char *msg = strsignal(-1);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Unknown signal", msg);
}

/* ============================================================================ */

TEST_SUITE(strcoll_l)

TEST(strcoll_l_basic) {
	ASSERT_EQ(0, strcoll_l("hello", "hello", __jacl_locale_current));
	ASSERT_TRUE(strcoll_l("abc", "abd", __jacl_locale_current) < 0);
}

TEST(strcoll_l_equal) {
	ASSERT_EQ(0, strcoll_l("hello", "hello", __jacl_locale_current));
}

TEST(strcoll_l_less) {
	ASSERT_TRUE(strcoll_l("abc", "abd", __jacl_locale_current) < 0);
}

TEST(strcoll_l_greater) {
	ASSERT_TRUE(strcoll_l("abd", "abc", __jacl_locale_current) > 0);
}

TEST(strcoll_l_null_a) {
	ASSERT_NE(0, strcoll_l(NULL, "a", __jacl_locale_current));
}

TEST(strcoll_l_null_b) {
	ASSERT_NE(0, strcoll_l("a", NULL, __jacl_locale_current));
}

TEST(strcoll_l_empty) {
	ASSERT_EQ(0, strcoll_l("", "", __jacl_locale_current));
	ASSERT_TRUE(strcoll_l("", "a", __jacl_locale_current) < 0);
}

/* ============================================================================ */

TEST_SUITE(strxfrm_l)

TEST(strxfrm_l_basic) {
	char dest[20];
	size_t ret = strxfrm_l(dest, "hello", 20, __jacl_locale_current);
	ASSERT_EQ(5, ret);
	ASSERT_STR_EQ("hello", dest);
}

TEST(strxfrm_l_null_dest) {
	ASSERT_EQ(5, strxfrm_l(NULL, "hello", 0, __jacl_locale_current));
}

TEST(strxfrm_l_null_src) {
	char dest[10];
	ASSERT_EQ(0, strxfrm_l(dest, NULL, 10, __jacl_locale_current));
}

TEST(strxfrm_l_small_buffer) {
	char dest[3];
	size_t ret = strxfrm_l(dest, "hello", 3, __jacl_locale_current);
	ASSERT_EQ(5, ret);
}

TEST(strxfrm_l_zero_n) {
	char dest[10] = "X";
	size_t ret = strxfrm_l(dest, "hello", 0, __jacl_locale_current);
	ASSERT_EQ(5, ret);
	ASSERT_EQ('X', dest[0]);
}

TEST(strxfrm_l_empty_src) {
	char dest[10];
	size_t ret = strxfrm_l(dest, "", 10, __jacl_locale_current);
	ASSERT_EQ(0, ret);
	ASSERT_EQ('\0', dest[0]);
}

/* ============================================================================ */

TEST_SUITE(strerror_l)

TEST(strerror_l_basic) {
	char *msg = strerror_l(ENOENT, __jacl_locale_current);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("No such file or directory", msg);
}

TEST(strerror_l_success) {
	char *msg = strerror_l(0, __jacl_locale_current);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Success", msg);
}

TEST(strerror_l_enomem) {
	char *msg = strerror_l(ENOMEM, __jacl_locale_current);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Out of memory", msg);
}

TEST(strerror_l_unknown) {
	char *msg = strerror_l(9999, __jacl_locale_current);
	ASSERT_NOT_NULL(msg);
	ASSERT_STR_EQ("Unknown error", msg);
}

/* ============================================================================ */

TEST_MAIN()
