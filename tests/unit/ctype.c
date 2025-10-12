/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <ctype.h>

TEST_TYPE(unit);
TEST_UNIT(ctype.h);

/* ============================================================================
 * ALPHANUMERIC TESTS
 * ============================================================================ */
TEST_SUITE(alphanumeric);

TEST(isalnum_letters) {
	ASSERT_TRUE(isalnum('a'));
	ASSERT_TRUE(isalnum('z'));
	ASSERT_TRUE(isalnum('A'));
	ASSERT_TRUE(isalnum('Z'));
	ASSERT_TRUE(isalnum('m'));
	ASSERT_TRUE(isalnum('M'));
}

TEST(isalnum_digits) {
	ASSERT_TRUE(isalnum('0'));
	ASSERT_TRUE(isalnum('9'));
	ASSERT_TRUE(isalnum('5'));
}

TEST(isalnum_non_alphanumeric) {
	ASSERT_FALSE(isalnum(' '));
	ASSERT_FALSE(isalnum('!'));
	ASSERT_FALSE(isalnum('@'));
	ASSERT_FALSE(isalnum('.'));
	ASSERT_FALSE(isalnum('\n'));
	ASSERT_FALSE(isalnum('\0'));
}

TEST(isalpha_letters) {
	ASSERT_TRUE(isalpha('a'));
	ASSERT_TRUE(isalpha('z'));
	ASSERT_TRUE(isalpha('A'));
	ASSERT_TRUE(isalpha('Z'));
	ASSERT_TRUE(isalpha('m'));
	ASSERT_TRUE(isalpha('M'));
}

TEST(isalpha_non_letters) {
	ASSERT_FALSE(isalpha('0'));
	ASSERT_FALSE(isalpha('9'));
	ASSERT_FALSE(isalpha(' '));
	ASSERT_FALSE(isalpha('!'));
	ASSERT_FALSE(isalpha('\n'));
}

/* ============================================================================
 * DIGIT TESTS
 * ============================================================================ */
TEST_SUITE(digits);

TEST(isdigit_valid) {
	ASSERT_TRUE(isdigit('0'));
	ASSERT_TRUE(isdigit('1'));
	ASSERT_TRUE(isdigit('5'));
	ASSERT_TRUE(isdigit('9'));
}

TEST(isdigit_invalid) {
	ASSERT_FALSE(isdigit('a'));
	ASSERT_FALSE(isdigit('A'));
	ASSERT_FALSE(isdigit(' '));
	ASSERT_FALSE(isdigit('\n'));
	ASSERT_FALSE(isdigit('/'));
	ASSERT_FALSE(isdigit(':'));
}

TEST(isxdigit_hex_digits) {
	ASSERT_TRUE(isxdigit('0'));
	ASSERT_TRUE(isxdigit('9'));
	ASSERT_TRUE(isxdigit('a'));
	ASSERT_TRUE(isxdigit('f'));
	ASSERT_TRUE(isxdigit('A'));
	ASSERT_TRUE(isxdigit('F'));
	ASSERT_TRUE(isxdigit('c'));
	ASSERT_TRUE(isxdigit('C'));
}

TEST(isxdigit_invalid) {
	ASSERT_FALSE(isxdigit('g'));
	ASSERT_FALSE(isxdigit('G'));
	ASSERT_FALSE(isxdigit('z'));
	ASSERT_FALSE(isxdigit(' '));
	ASSERT_FALSE(isxdigit('\n'));
}

/* ============================================================================
 * CASE TESTS
 * ============================================================================ */
TEST_SUITE(case_tests);

TEST(isupper_uppercase) {
	ASSERT_TRUE(isupper('A'));
	ASSERT_TRUE(isupper('Z'));
	ASSERT_TRUE(isupper('M'));
}

TEST(isupper_not_uppercase) {
	ASSERT_FALSE(isupper('a'));
	ASSERT_FALSE(isupper('z'));
	ASSERT_FALSE(isupper('0'));
	ASSERT_FALSE(isupper(' '));
	ASSERT_FALSE(isupper('\n'));
}

TEST(islower_lowercase) {
	ASSERT_TRUE(islower('a'));
	ASSERT_TRUE(islower('z'));
	ASSERT_TRUE(islower('m'));
}

TEST(islower_not_lowercase) {
	ASSERT_FALSE(islower('A'));
	ASSERT_FALSE(islower('Z'));
	ASSERT_FALSE(islower('0'));
	ASSERT_FALSE(islower(' '));
	ASSERT_FALSE(islower('\n'));
}

/* ============================================================================
 * CASE CONVERSION TESTS
 * ============================================================================ */
TEST_SUITE(case_conversion);

TEST(tolower_uppercase) {
	ASSERT_EQ('a', tolower('A'));
	ASSERT_EQ('z', tolower('Z'));
	ASSERT_EQ('m', tolower('M'));
}

TEST(tolower_already_lower) {
	ASSERT_EQ('a', tolower('a'));
	ASSERT_EQ('z', tolower('z'));
	ASSERT_EQ('m', tolower('m'));
}

TEST(tolower_non_letters) {
	ASSERT_EQ('0', tolower('0'));
	ASSERT_EQ('9', tolower('9'));
	ASSERT_EQ(' ', tolower(' '));
	ASSERT_EQ('!', tolower('!'));
	ASSERT_EQ('\n', tolower('\n'));
}

TEST(toupper_lowercase) {
	ASSERT_EQ('A', toupper('a'));
	ASSERT_EQ('Z', toupper('z'));
	ASSERT_EQ('M', toupper('m'));
}

TEST(toupper_already_upper) {
	ASSERT_EQ('A', toupper('A'));
	ASSERT_EQ('Z', toupper('Z'));
	ASSERT_EQ('M', toupper('M'));
}

TEST(toupper_non_letters) {
	ASSERT_EQ('0', toupper('0'));
	ASSERT_EQ('9', toupper('9'));
	ASSERT_EQ(' ', toupper(' '));
	ASSERT_EQ('!', toupper('!'));
	ASSERT_EQ('\n', toupper('\n'));
}

/* ============================================================================
 * WHITESPACE TESTS
 * ============================================================================ */
TEST_SUITE(whitespace);

TEST(isspace_whitespace) {
	ASSERT_TRUE(isspace(' '));
	ASSERT_TRUE(isspace('\t'));
	ASSERT_TRUE(isspace('\n'));
	ASSERT_TRUE(isspace('\r'));
	ASSERT_TRUE(isspace('\f'));
	ASSERT_TRUE(isspace('\v'));
}

TEST(isspace_non_whitespace) {
	ASSERT_FALSE(isspace('a'));
	ASSERT_FALSE(isspace('0'));
	ASSERT_FALSE(isspace('!'));
	ASSERT_FALSE(isspace('\0'));
}

#if JACL_HAS_C99
TEST(isblank_horizontal_space) {
	ASSERT_TRUE(isblank(' '));
	ASSERT_TRUE(isblank('\t'));
}

TEST(isblank_not_blank) {
	ASSERT_FALSE(isblank('\n'));
	ASSERT_FALSE(isblank('\r'));
	ASSERT_FALSE(isblank('\f'));
	ASSERT_FALSE(isblank('\v'));
	ASSERT_FALSE(isblank('a'));
	ASSERT_FALSE(isblank('0'));
}
#endif

/* ============================================================================
 * CONTROL CHARACTER TESTS
 * ============================================================================ */
TEST_SUITE(control);

TEST(iscntrl_control_chars) {
	ASSERT_TRUE(iscntrl('\0'));
	ASSERT_TRUE(iscntrl('\t'));
	ASSERT_TRUE(iscntrl('\n'));
	ASSERT_TRUE(iscntrl('\r'));
	ASSERT_TRUE(iscntrl('\x1B'));  // ESC
	ASSERT_TRUE(iscntrl(127));     // DEL
}

TEST(iscntrl_not_control) {
	ASSERT_FALSE(iscntrl(' '));
	ASSERT_FALSE(iscntrl('a'));
	ASSERT_FALSE(iscntrl('0'));
	ASSERT_FALSE(iscntrl('~'));
}

/* ============================================================================
 * PRINTABLE TESTS
 * ============================================================================ */
TEST_SUITE(printable);

TEST(isprint_printable) {
	ASSERT_TRUE(isprint(' '));
	ASSERT_TRUE(isprint('!'));
	ASSERT_TRUE(isprint('a'));
	ASSERT_TRUE(isprint('Z'));
	ASSERT_TRUE(isprint('0'));
	ASSERT_TRUE(isprint('~'));
}

TEST(isprint_not_printable) {
	ASSERT_FALSE(isprint('\0'));
	ASSERT_FALSE(isprint('\t'));
	ASSERT_FALSE(isprint('\n'));
	ASSERT_FALSE(isprint(127));
	ASSERT_FALSE(isprint(31));
}

TEST(isgraph_graphical) {
	ASSERT_TRUE(isgraph('!'));
	ASSERT_TRUE(isgraph('a'));
	ASSERT_TRUE(isgraph('Z'));
	ASSERT_TRUE(isgraph('0'));
	ASSERT_TRUE(isgraph('~'));
	ASSERT_TRUE(isgraph('@'));
}

TEST(isgraph_not_graphical) {
	ASSERT_FALSE(isgraph(' '));
	ASSERT_FALSE(isgraph('\t'));
	ASSERT_FALSE(isgraph('\n'));
	ASSERT_FALSE(isgraph('\0'));
	ASSERT_FALSE(isgraph(127));
}

/* ============================================================================
 * PUNCTUATION TESTS
 * ============================================================================ */
TEST_SUITE(punctuation);

TEST(ispunct_punctuation) {
	ASSERT_TRUE(ispunct('!'));
	ASSERT_TRUE(ispunct('.'));
	ASSERT_TRUE(ispunct(','));
	ASSERT_TRUE(ispunct('?'));
	ASSERT_TRUE(ispunct(';'));
	ASSERT_TRUE(ispunct(':'));
	ASSERT_TRUE(ispunct('-'));
	ASSERT_TRUE(ispunct('@'));
	ASSERT_TRUE(ispunct('#'));
}

TEST(ispunct_not_punctuation) {
	ASSERT_FALSE(ispunct('a'));
	ASSERT_FALSE(ispunct('Z'));
	ASSERT_FALSE(ispunct('0'));
	ASSERT_FALSE(ispunct('9'));
	ASSERT_FALSE(ispunct(' '));
	ASSERT_FALSE(ispunct('\n'));
}

/* ============================================================================
 * BOUNDARY TESTS
 * ============================================================================ */
TEST_SUITE(boundaries);

TEST(ascii_boundaries) {
	// First printable
	ASSERT_TRUE(isprint(' '));
	ASSERT_EQ(' ', 32);
	
	// Last printable
	ASSERT_TRUE(isprint('~'));
	ASSERT_EQ('~', 126);
	
	// DEL boundary
	ASSERT_TRUE(iscntrl(127));
	ASSERT_FALSE(isprint(127));
}

TEST(letter_boundaries) {
	ASSERT_TRUE(isalpha('A'));
	ASSERT_TRUE(isalpha('Z'));
	ASSERT_FALSE(isalpha('@'));
	ASSERT_FALSE(isalpha('['));
	
	ASSERT_TRUE(isalpha('a'));
	ASSERT_TRUE(isalpha('z'));
	ASSERT_FALSE(isalpha('`'));
	ASSERT_FALSE(isalpha('{'));
}

TEST(digit_boundaries) {
	ASSERT_TRUE(isdigit('0'));
	ASSERT_TRUE(isdigit('9'));
	ASSERT_FALSE(isdigit('/'));
	ASSERT_FALSE(isdigit(':'));
}

/* ============================================================================
 * COMBINED TESTS
 * ============================================================================ */
TEST_SUITE(combined);

TEST(string_classification) {
	const char *str = "Hello123!";
	
	ASSERT_TRUE(isupper(str[0]));
	ASSERT_TRUE(islower(str[1]));
	ASSERT_TRUE(isalpha(str[0]));
	ASSERT_TRUE(isdigit(str[5]));
	ASSERT_TRUE(ispunct(str[8]));
	ASSERT_TRUE(isalnum(str[0]));
	ASSERT_TRUE(isalnum(str[5]));
}

TEST(case_conversion_string) {
	char str[] = "HeLLo WoRLd!";
	
	for (int i = 0; str[i]; i++) {
		if (isupper(str[i])) {
			str[i] = tolower(str[i]);
		} else if (islower(str[i])) {
			str[i] = toupper(str[i]);
		}
	}
	
	ASSERT_STR_EQ("hEllO wOrld!", str);
}

TEST(whitespace_detection) {
	const char *str = "a b\tc\nd";
	
	ASSERT_FALSE(isspace(str[0]));
	ASSERT_TRUE(isspace(str[1]));
	ASSERT_FALSE(isspace(str[2]));
	ASSERT_TRUE(isspace(str[3]));
	ASSERT_FALSE(isspace(str[4]));
	ASSERT_TRUE(isspace(str[5]));
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(null_character) {
	ASSERT_FALSE(isalnum('\0'));
	ASSERT_FALSE(isalpha('\0'));
	ASSERT_FALSE(isdigit('\0'));
	ASSERT_TRUE(iscntrl('\0'));
	ASSERT_FALSE(isspace('\0'));
	ASSERT_FALSE(isprint('\0'));
	ASSERT_FALSE(isgraph('\0'));
	ASSERT_FALSE(ispunct('\0'));
}

TEST(negative_values) {
	// Test with negative int values (EOF is typically -1)
	ASSERT_FALSE(isalnum(-1));
	ASSERT_FALSE(isalpha(-1));
	ASSERT_FALSE(isdigit(-1));
}

TEST(high_ascii) {
	// Test behavior with values > 127 (extended ASCII)
	// These should all return false for standard ASCII functions
	ASSERT_FALSE(isalnum(128));
	ASSERT_FALSE(isalpha(200));
	ASSERT_FALSE(isdigit(255));
}

TEST(all_letters) {
	// Verify all 52 letters work
	for (char c = 'A'; c <= 'Z'; c++) {
		ASSERT_TRUE(isalpha(c));
		ASSERT_TRUE(isupper(c));
		ASSERT_FALSE(islower(c));
		ASSERT_EQ(c + 32, tolower(c));
	}
	
	for (char c = 'a'; c <= 'z'; c++) {
		ASSERT_TRUE(isalpha(c));
		ASSERT_TRUE(islower(c));
		ASSERT_FALSE(isupper(c));
		ASSERT_EQ(c - 32, toupper(c));
	}
}

TEST(all_digits) {
	for (char c = '0'; c <= '9'; c++) {
		ASSERT_TRUE(isdigit(c));
		ASSERT_TRUE(isxdigit(c));
		ASSERT_TRUE(isalnum(c));
	}
}

/* ============================================================================
 * PERFORMANCE TEST
 * ============================================================================ */
TEST_SUITE(performance);

TEST(batch_classification) {
	char test_string[256];
	for (int i = 0; i < 256; i++) {
		test_string[i] = (char)i;
	}
	
	int alpha_count = 0;
	int digit_count = 0;
	int space_count = 0;
	
	for (int i = 0; i < 256; i++) {
		if (isalpha(test_string[i])) alpha_count++;
		if (isdigit(test_string[i])) digit_count++;
		if (isspace(test_string[i])) space_count++;
	}
	
	ASSERT_EQ(52, alpha_count);  // A-Z + a-z
	ASSERT_EQ(10, digit_count);  // 0-9
	ASSERT_EQ(6, space_count);   // space, tab, newline, etc.
}

TEST_MAIN()

