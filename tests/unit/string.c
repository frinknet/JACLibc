/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>

TEST_SUITE(string);

/* ===== Memory Operations Tests ===== */
TEST(memcpy_basic_copy) {
		char src[10] = "hello";
		char dest[10];
		
		void *result = memcpy(dest, src, 6);
		ASSERT_EQ(dest, result);	// Should return dest
		ASSERT_EQ(0, strcmp(dest, "hello"));
		
		// Test partial copy
		memcpy(dest, "world", 3);
		dest[3] = '\0';
		ASSERT_EQ(0, strcmp(dest, "wor"));
}

TEST(memcpy_overlapping_undefined) {
		// memcpy behavior with overlapping regions is undefined
		// We test that it doesn't crash, but don't rely on results
		char buffer[10] = "abcdef";
		memcpy(buffer + 1, buffer, 3);	// Should use memmove for this
		// Just verify it doesn't crash
}

TEST(memset_fill_memory) {
		char buffer[10];
		
		void *result = memset(buffer, 'A', 5);
		ASSERT_EQ(buffer, result);
		buffer[5] = '\0';
		ASSERT_EQ(0, strcmp(buffer, "AAAAA"));
		
		// Test zero fill
		memset(buffer, 0, 10);
		for (int i = 0; i < 10; i++) {
				ASSERT_EQ(0, buffer[i]);
		}
}

TEST(memmove_safe_overlap) {
		char buffer[20] = "hello world";
		
		// Move forward (overlapping)
		void *result = memmove(buffer + 2, buffer, 5);
		ASSERT_EQ(buffer + 2, result);
		buffer[7] = '\0';
		ASSERT_EQ(0, strcmp(buffer + 2, "hello"));
		
		// Move backward (overlapping) 
		strcpy(buffer, "abcdefgh");
		memmove(buffer, buffer + 3, 5);
		buffer[5] = '\0';
		ASSERT_EQ(0, strcmp(buffer, "defgh"));
}

TEST(memcmp_comparison) {
		ASSERT_EQ(0, memcmp("hello", "hello", 5));
		ASSERT_TRUE(memcmp("abc", "abd", 3) < 0);
		ASSERT_TRUE(memcmp("abd", "abc", 3) > 0);
		
		// Compare with embedded nulls
		char a[5] = {'a', '\0', 'b', 'c', 'd'};
		char b[5] = {'a', '\0', 'b', 'c', 'e'};
		ASSERT_TRUE(memcmp(a, b, 5) < 0);
		
		// Zero length comparison
		ASSERT_EQ(0, memcmp("different", "strings", 0));
}

TEST(memchr_find_byte) {
		const char *str = "hello world";
		
		ASSERT_EQ(str + 4, memchr(str, 'o', 11));			 // First 'o'
		ASSERT_EQ(str + 0, memchr(str, 'h', 11));			 // First char
		ASSERT_EQ(str + 10, memchr(str, 'd', 11));		 // Last char
		ASSERT_EQ(NULL, memchr(str, 'x', 11));				 // Not found
		ASSERT_EQ(NULL, memchr(str, 'o', 3));					 // Beyond search range
}

#ifdef _GNU_SOURCE
TEST(memmem_find_substring) {
		const char *haystack = "hello world hello";
		
		ASSERT_EQ(haystack + 0, memmem(haystack, 17, "hello", 5));	 // First match
		ASSERT_EQ(haystack + 6, memmem(haystack, 17, "world", 5));	 // Middle match
		ASSERT_EQ(NULL, memmem(haystack, 17, "goodbye", 7));				 // Not found
		ASSERT_EQ(NULL, memmem(haystack, 5, "world", 5));						 // Beyond range
		
		// Empty needle
		ASSERT_EQ(haystack, memmem(haystack, 17, "", 0));
		
		// Needle larger than haystack
		ASSERT_EQ(NULL, memmem("hi", 2, "hello", 5));
}
#endif

/* ===== String Length and Copy Tests ===== */
TEST(strlen_basic_length) {
		ASSERT_EQ(0, strlen(""));
		ASSERT_EQ(5, strlen("hello"));
		ASSERT_EQ(11, strlen("hello world"));
		
		// String with embedded content
		char str[10] = {'a', 'b', '\0', 'c', 'd'};
		ASSERT_EQ(2, strlen(str));	// Stops at first null
}

TEST(strnlen_limited_length) {
		ASSERT_EQ(0, strnlen("", 10));
		ASSERT_EQ(5, strnlen("hello", 10));					 // String shorter than limit
		ASSERT_EQ(3, strnlen("hello", 3));					 // Limit shorter than string
		ASSERT_EQ(0, strnlen("test", 0));						 // Zero limit
}

TEST(strcpy_copy_string) {
		char dest[20];
		
		char *result = strcpy(dest, "hello");
		ASSERT_EQ(dest, result);
		ASSERT_EQ(0, strcmp(dest, "hello"));
		
		// Empty string
		strcpy(dest, "");
		ASSERT_EQ(0, strcmp(dest, ""));
}

TEST(strncpy_safe_copy) {
		char dest[10];
		
		// Normal case - string fits
		char *result = strncpy(dest, "hello", 10);
		ASSERT_EQ(dest, result);
		ASSERT_EQ(0, strcmp(dest, "hello"));
		
		// String exactly fits
		strncpy(dest, "12345", 5);
		dest[5] = '\0';  // Need manual null termination
		ASSERT_EQ(0, strcmp(dest, "12345"));
		
		// String truncated, remaining filled with nulls
		memset(dest, 'X', 10);
		strncpy(dest, "hi", 5);
		ASSERT_EQ('h', dest[0]);
		ASSERT_EQ('i', dest[1]);
		ASSERT_EQ('\0', dest[2]);
		ASSERT_EQ('\0', dest[3]);
		ASSERT_EQ('\0', dest[4]);
}

TEST(strcat_concatenation) {
		char dest[20] = "hello";
		
		char *result = strcat(dest, " world");
		ASSERT_EQ(dest, result);
		ASSERT_EQ(0, strcmp(dest, "hello world"));
		
		// Concatenate to empty string
		strcpy(dest, "");
		strcat(dest, "test");
		ASSERT_EQ(0, strcmp(dest, "test"));
}

TEST(strncat_safe_concatenation) {
		char dest[20] = "hello";
		
		char *result = strncat(dest, " world!", 6);  // Takes exactly 6 chars: " world"
		ASSERT_EQ(dest, result);
		ASSERT_EQ(0, strcmp(dest, "hello world"));	// Fixed expectation
		
		// Add exactly n characters
		strcpy(dest, "test");
		strncat(dest, "12345", 3);
		ASSERT_EQ(0, strcmp(dest, "test123"));
}

/* ===== String Comparison Tests ===== */
TEST(strcmp_string_comparison) {
		ASSERT_EQ(0, strcmp("hello", "hello"));
		ASSERT_TRUE(strcmp("abc", "abd") < 0);
		ASSERT_TRUE(strcmp("abd", "abc") > 0);
		ASSERT_TRUE(strcmp("", "a") < 0);
		ASSERT_TRUE(strcmp("a", "") > 0);
		ASSERT_EQ(0, strcmp("", ""));
}

TEST(strncmp_limited_comparison) {
		ASSERT_EQ(0, strncmp("hello", "hello", 5));
		ASSERT_EQ(0, strncmp("hello123", "hello456", 5));  // Equal within n
		ASSERT_TRUE(strncmp("abc", "abd", 3) < 0);
		ASSERT_TRUE(strncmp("abd", "abc", 3) > 0);
		
		// Zero length comparison
		ASSERT_EQ(0, strncmp("different", "strings", 0));
}

TEST(strcoll_collation) {
		// Your implementation just calls strcmp
		ASSERT_EQ(0, strcoll("hello", "hello"));
		ASSERT_TRUE(strcoll("abc", "abd") < 0);
}

TEST(strxfrm_transformation) {
		char dest[20];
		
		size_t result = strxfrm(dest, "hello", 20);
		ASSERT_EQ(5, result);  // Should return length of source
		ASSERT_EQ(0, strcmp(dest, "hello"));
		
		// Test with small buffer
		result = strxfrm(dest, "hello world", 5);
		ASSERT_EQ(11, result);	// Still returns full length
		// dest content undefined when n < src length
}

/* ===== Error Handling Tests ===== */
TEST(strerror_error_messages) {
		char *msg0 = strerror(0);
		ASSERT_NE(NULL, msg0);
		ASSERT_EQ(0, strcmp(msg0, "Success"));
		
		char *msg1 = strerror(1);
		ASSERT_NE(NULL, msg1);
		ASSERT_EQ(0, strcmp(msg1, "Perm"));
		
		char *msg_unknown = strerror(999);
		ASSERT_NE(NULL, msg_unknown);
		ASSERT_EQ(0, strcmp(msg_unknown, "Unknown"));
}

TEST(strerror_r_thread_safe) {
		char buffer[20];
		
		int result = strerror_r(0, buffer, 20);
		ASSERT_EQ(0, result);
		ASSERT_EQ(0, strcmp(buffer, "Success"));
		
		// Test truncation
		result = strerror_r(0, buffer, 3);
		ASSERT_EQ(0, result);
		buffer[2] = '\0';
		ASSERT_EQ(0, strcmp(buffer, "Su"));
}

/* ===== String Search Tests ===== */
TEST(strchr_find_character) {
		const char *str = "hello world";
		
		ASSERT_EQ(str + 4, strchr(str, 'o'));					 // First 'o'
		ASSERT_EQ(str + 0, strchr(str, 'h'));					 // First char
		ASSERT_EQ(str + 10, strchr(str, 'd'));				 // Last char
		ASSERT_EQ(NULL, strchr(str, 'x'));						 // Not found
		
		// Find null terminator
		ASSERT_EQ(str + 11, strchr(str, '\0'));
}

TEST(strrchr_find_last_character) {
		const char *str = "hello world";
		
		ASSERT_EQ(str + 7, strrchr(str, 'o'));				 // Last 'o'
		ASSERT_EQ(str + 9, strrchr(str, 'l'));				 // Last 'l'
		ASSERT_EQ(str + 0, strrchr(str, 'h'));				 // Only 'h'
		ASSERT_EQ(NULL, strrchr(str, 'x'));						 // Not found
}

TEST(strstr_find_substring) {
		const char *str = "hello world hello";
		
		ASSERT_EQ(str + 0, strstr(str, "hello"));			 // First match
		ASSERT_EQ(str + 6, strstr(str, "world"));			 // Middle match
		ASSERT_EQ(NULL, strstr(str, "goodbye"));			 // Not found
		
		// Empty needle
		ASSERT_EQ(str, strstr(str, ""));
		
		// Find substring that appears later
		ASSERT_EQ(str + 12, strstr(str + 6, "hello")); // Search from middle for second "hello"
}

TEST(strspn_span_characters) {
		ASSERT_EQ(0, strspn("hello", "xyz"));					 // No match
		ASSERT_EQ(4, strspn("hello", "hel"));					 // "hell" matches (not just "he")
		ASSERT_EQ(5, strspn("hello", "helo"));				 // All match
		ASSERT_EQ(3, strspn("abcdef", "cab"));				 // "abc" matches
}

TEST(strcspn_complement_span) {
		ASSERT_EQ(5, strcspn("hello", "xyz"));				 // No reject chars
		ASSERT_EQ(2, strcspn("hello", "l"));					 // Stop at first 'l'
		ASSERT_EQ(0, strcspn("hello", "h"));					 // Stop immediately
		ASSERT_EQ(3, strcspn("abcdef", "def"));				 // Stop at 'd'
}

TEST(strpbrk_find_any_character) {
		const char *str = "hello world";
		
		ASSERT_EQ(str + 2, strpbrk(str, "l"));				 // First 'l'
		ASSERT_EQ(str + 2, strpbrk(str, "xl"));				 // First of any
		ASSERT_EQ(str + 4, strpbrk(str, "or"));				 // First 'o'
		ASSERT_EQ(NULL, strpbrk(str, "xyz"));					 // None found
}

TEST(strtok_tokenization) {
		char str[] = "hello,world;test:end";
		
		char *token = strtok(str, ",;:");
		ASSERT_NE(NULL, token);
		ASSERT_EQ(0, strcmp(token, "hello"));
		
		token = strtok(NULL, ",;:");
		ASSERT_NE(NULL, token);
		ASSERT_EQ(0, strcmp(token, "world"));
		
		token = strtok(NULL, ",;:");
		ASSERT_NE(NULL, token);
		ASSERT_EQ(0, strcmp(token, "test"));
		
		token = strtok(NULL, ",;:");
		ASSERT_NE(NULL, token);
		ASSERT_EQ(0, strcmp(token, "end"));
		
		token = strtok(NULL, ",;:");
		ASSERT_EQ(NULL, token);  // No more tokens
}

TEST(strtok_r_reentrant_tokenization) {
		char str1[] = "a,b,c";
		char str2[] = "x:y:z";
		char *save1, *save2;
		
		// Interleave two tokenizations
		char *t1 = strtok_r(str1, ",", &save1);
		char *t2 = strtok_r(str2, ":", &save2);
		
		ASSERT_EQ(0, strcmp(t1, "a"));
		ASSERT_EQ(0, strcmp(t2, "x"));
		
		t1 = strtok_r(NULL, ",", &save1);
		t2 = strtok_r(NULL, ":", &save2);
		
		ASSERT_EQ(0, strcmp(t1, "b"));
		ASSERT_EQ(0, strcmp(t2, "y"));
}

/* ===== BSD-style Safe Functions Tests ===== */
#if defined(_BSD_SOURCE) || defined(_SUSV2)
TEST(strlcpy_safe_copy) {
		char dest[10];
		
		// String fits
		size_t result = strlcpy(dest, "hello", 10);
		ASSERT_EQ(5, result);  // Length of source
		ASSERT_EQ(0, strcmp(dest, "hello"));
		
		// String truncated
		result = strlcpy(dest, "very long string", 5);
		ASSERT_EQ(16, result);	// Full source length
		ASSERT_EQ(0, strcmp(dest, "very"));  // Truncated but null-terminated
}

TEST(strlcat_safe_concatenation) {
		char dest[10] = "hi";
		
		// Concatenation fits
		size_t result = strlcat(dest, " there", 10);
		ASSERT_EQ(8, result);  // Final length would be
		ASSERT_EQ(0, strcmp(dest, "hi there"));
		
		// Concatenation truncated
		strcpy(dest, "hello");
		result = strlcat(dest, " world", 8);
		ASSERT_EQ(11, result);	// Would-be final length
		ASSERT_EQ(0, strcmp(dest, "hello w"));	// Truncated
}
#endif

/* ===== POSIX basename Tests ===== */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
TEST(basename_path_extraction) {
		ASSERT_EQ(0, strcmp(basename("/usr/bin/test"), "test"));
		ASSERT_EQ(0, strcmp(basename("/usr/bin/"), ""));
		ASSERT_EQ(0, strcmp(basename("filename"), "filename"));
		ASSERT_EQ(0, strcmp(basename(""), ""));
}
#endif

/* ===== Edge Cases and Integration Tests ===== */
TEST(empty_string_handling) {
		char empty[10] = "";
		
		ASSERT_EQ(0, strlen(empty));
		ASSERT_EQ(empty, strcpy(empty, ""));
		ASSERT_EQ(0, strcmp(empty, ""));
		ASSERT_EQ(NULL, strchr("", 'x'));
		ASSERT_EQ(empty, strstr(empty, ""));
}

TEST(null_terminator_handling) {
		char str[10] = {'a', 'b', 'c', '\0', 'd', 'e', 'f', '\0'};
		
		ASSERT_EQ(3, strlen(str));	// Stops at first null
		ASSERT_EQ(str + 3, strchr(str, '\0'));	// Find null terminator
		ASSERT_EQ(str + 3, strrchr(str, '\0')); // Find last null in logical string
}


TEST(memory_boundary_conditions) {
		// Test with single character
		char single[2] = "a";
		ASSERT_EQ(1, strlen(single));
		ASSERT_EQ(single, strchr(single, 'a'));
		
		// Test with exactly fitting strings
		char exact[6];
		strcpy(exact, "hello");
		ASSERT_EQ(5, strlen(exact));
		ASSERT_EQ(0, strcmp(exact, "hello"));
}

TEST(character_range_handling) {
		// Test with full character range
		unsigned char str[3] = {255, 128, 0};
		
		ASSERT_EQ(str + 0, memchr(str, 255, 3));
		ASSERT_EQ(str + 1, memchr(str, 128, 3));
		ASSERT_EQ(str + 2, memchr(str, 0, 3));
}

TEST(large_string_operations) {
		// Test with longer strings to exercise implementation
		char large1[1000];
		char large2[1000];
		
		memset(large1, 'A', 999);
		large1[999] = '\0';
		
		strcpy(large2, large1);
		ASSERT_EQ(999, strlen(large2));
		ASSERT_EQ(0, strcmp(large1, large2));
		
		// Test search in large string
		large1[500] = 'X';
		ASSERT_EQ(large1 + 500, strchr(large1, 'X'));
}

TEST_MAIN()

