/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <ctype.h>

TEST_TYPE(unit);
TEST_UNIT(ctype.h);

/* ============================================================================
 * isalnum
 * ============================================================================ */
TEST_SUITE(isalnum);

TEST(isalnum_letters) {
	ASSERT_TRUE(isalnum('a'));
	ASSERT_TRUE(isalnum('z'));
	ASSERT_TRUE(isalnum('A'));
	ASSERT_TRUE(isalnum('Z'));
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

TEST(isalnum_boundaries) {
	ASSERT_FALSE(isalnum('/'));
	ASSERT_FALSE(isalnum(':'));
	ASSERT_FALSE(isalnum('`'));
	ASSERT_FALSE(isalnum('{'));
}

TEST(isalnum_null) {
	ASSERT_FALSE(isalnum('\0'));
}

TEST(isalnum_negative) {
	ASSERT_FALSE(isalnum(-1));
}

TEST(isalnum_high_ascii) {
	ASSERT_FALSE(isalnum(128));
	ASSERT_FALSE(isalnum(255));
}

/* ============================================================================
 * isalpha
 * ============================================================================ */
TEST_SUITE(isalpha);

TEST(isalpha_letters) {
	ASSERT_TRUE(isalpha('a'));
	ASSERT_TRUE(isalpha('z'));
	ASSERT_TRUE(isalpha('A'));
	ASSERT_TRUE(isalpha('Z'));
}

TEST(isalpha_non_letters) {
	ASSERT_FALSE(isalpha('0'));
	ASSERT_FALSE(isalpha('9'));
	ASSERT_FALSE(isalpha(' '));
	ASSERT_FALSE(isalpha('!'));
	ASSERT_FALSE(isalpha('\n'));
}

TEST(isalpha_boundaries) {
	ASSERT_FALSE(isalpha('@'));
	ASSERT_FALSE(isalpha('['));
	ASSERT_FALSE(isalpha('`'));
	ASSERT_FALSE(isalpha('{'));
}

TEST(isalpha_null) {
	ASSERT_FALSE(isalpha('\0'));
}

TEST(isalpha_negative) {
	ASSERT_FALSE(isalpha(-1));
}

TEST(isalpha_high_ascii) {
	ASSERT_FALSE(isalpha(128));
	ASSERT_FALSE(isalpha(255));
}

/* ============================================================================
 * isdigit
 * ============================================================================ */
TEST_SUITE(isdigit);

TEST(isdigit_valid) {
	ASSERT_TRUE(isdigit('0'));
	ASSERT_TRUE(isdigit('9'));
	ASSERT_TRUE(isdigit('5'));
}

TEST(isdigit_invalid) {
	ASSERT_FALSE(isdigit('a'));
	ASSERT_FALSE(isdigit('A'));
	ASSERT_FALSE(isdigit(' '));
	ASSERT_FALSE(isdigit('\n'));
}

TEST(isdigit_boundaries) {
	ASSERT_FALSE(isdigit('/'));
	ASSERT_FALSE(isdigit(':'));
}

TEST(isdigit_null) {
	ASSERT_FALSE(isdigit('\0'));
}

TEST(isdigit_negative) {
	ASSERT_FALSE(isdigit(-1));
}

TEST(isdigit_high_ascii) {
	ASSERT_FALSE(isdigit(128));
	ASSERT_FALSE(isdigit(255));
}

/* ============================================================================
 * isxdigit
 * ============================================================================ */
TEST_SUITE(isxdigit);

TEST(isxdigit_hex_digits) {
	ASSERT_TRUE(isxdigit('0'));
	ASSERT_TRUE(isxdigit('9'));
	ASSERT_TRUE(isxdigit('a'));
	ASSERT_TRUE(isxdigit('f'));
	ASSERT_TRUE(isxdigit('A'));
	ASSERT_TRUE(isxdigit('F'));
}

TEST(isxdigit_invalid) {
	ASSERT_FALSE(isxdigit('g'));
	ASSERT_FALSE(isxdigit('G'));
	ASSERT_FALSE(isxdigit('z'));
	ASSERT_FALSE(isxdigit(' '));
}

TEST(isxdigit_boundaries) {
	ASSERT_FALSE(isxdigit('/'));
	ASSERT_FALSE(isxdigit(':'));
	ASSERT_FALSE(isxdigit('@'));
	ASSERT_FALSE(isxdigit('`'));
}

TEST(isxdigit_null) {
	ASSERT_FALSE(isxdigit('\0'));
}

TEST(isxdigit_negative) {
	ASSERT_FALSE(isxdigit(-1));
}

TEST(isxdigit_high_ascii) {
	ASSERT_FALSE(isxdigit(128));
	ASSERT_FALSE(isxdigit(255));
}

/* ============================================================================
 * isupper
 * ============================================================================ */
TEST_SUITE(isupper);

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
}

TEST(isupper_boundaries) {
	ASSERT_FALSE(isupper('@'));
	ASSERT_FALSE(isupper('['));
}

TEST(isupper_null) {
	ASSERT_FALSE(isupper('\0'));
}

TEST(isupper_negative) {
	ASSERT_FALSE(isupper(-1));
}

TEST(isupper_high_ascii) {
	ASSERT_FALSE(isupper(128));
	ASSERT_FALSE(isupper(255));
}

/* ============================================================================
 * islower
 * ============================================================================ */
TEST_SUITE(islower);

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
}

TEST(islower_boundaries) {
	ASSERT_FALSE(islower('`'));
	ASSERT_FALSE(islower('{'));
}

TEST(islower_null) {
	ASSERT_FALSE(islower('\0'));
}

TEST(islower_negative) {
	ASSERT_FALSE(islower(-1));
}

TEST(islower_high_ascii) {
	ASSERT_FALSE(islower(128));
	ASSERT_FALSE(islower(255));
}

/* ============================================================================
 * tolower
 * ============================================================================ */
TEST_SUITE(tolower);

TEST(tolower_uppercase) {
	ASSERT_EQ('a', tolower('A'));
	ASSERT_EQ('z', tolower('Z'));
	ASSERT_EQ('m', tolower('M'));
}

TEST(tolower_already_lower) {
	ASSERT_EQ('a', tolower('a'));
	ASSERT_EQ('z', tolower('z'));
}

TEST(tolower_non_letters) {
	ASSERT_EQ('0', tolower('0'));
	ASSERT_EQ(' ', tolower(' '));
	ASSERT_EQ('!', tolower('!'));
	ASSERT_EQ('\n', tolower('\n'));
}

TEST(tolower_null) {
	ASSERT_EQ('\0', tolower('\0'));
}

TEST(tolower_negative) {
	ASSERT_EQ(-1, tolower(-1));
}

TEST(tolower_high_ascii) {
	ASSERT_EQ(128, tolower(128));
	ASSERT_EQ(255, tolower(255));
}

/* ============================================================================
 * toupper
 * ============================================================================ */
TEST_SUITE(toupper);

TEST(toupper_lowercase) {
	ASSERT_EQ('A', toupper('a'));
	ASSERT_EQ('Z', toupper('z'));
	ASSERT_EQ('M', toupper('m'));
}

TEST(toupper_already_upper) {
	ASSERT_EQ('A', toupper('A'));
	ASSERT_EQ('Z', toupper('Z'));
}

TEST(toupper_non_letters) {
	ASSERT_EQ('0', toupper('0'));
	ASSERT_EQ(' ', toupper(' '));
	ASSERT_EQ('!', toupper('!'));
	ASSERT_EQ('\n', toupper('\n'));
}

TEST(toupper_null) {
	ASSERT_EQ('\0', toupper('\0'));
}

TEST(toupper_negative) {
	ASSERT_EQ(-1, toupper(-1));
}

TEST(toupper_high_ascii) {
	ASSERT_EQ(128, toupper(128));
	ASSERT_EQ(255, toupper(255));
}

/* ============================================================================
 * isspace
 * ============================================================================ */
TEST_SUITE(isspace);

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

TEST(isspace_null) {
	ASSERT_FALSE(isspace('\0'));
}

TEST(isspace_negative) {
	ASSERT_FALSE(isspace(-1));
}

TEST(isspace_high_ascii) {
	ASSERT_FALSE(isspace(128));
	ASSERT_FALSE(isspace(255));
}

/* ============================================================================
 * isblank (C99)
 * ============================================================================ */
#if JACL_HAS_C99
TEST_SUITE(isblank);

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

TEST(isblank_null) {
	ASSERT_FALSE(isblank('\0'));
}

TEST(isblank_negative) {
	ASSERT_FALSE(isblank(-1));
}

TEST(isblank_high_ascii) {
	ASSERT_FALSE(isblank(128));
	ASSERT_FALSE(isblank(255));
}
#endif

/* ============================================================================
 * iscntrl
 * ============================================================================ */
TEST_SUITE(iscntrl);

TEST(iscntrl_control_chars) {
	ASSERT_TRUE(iscntrl('\0'));
	ASSERT_TRUE(iscntrl('\t'));
	ASSERT_TRUE(iscntrl('\n'));
	ASSERT_TRUE(iscntrl('\r'));
	ASSERT_TRUE(iscntrl('\x1B'));
	ASSERT_TRUE(iscntrl(127));
}

TEST(iscntrl_not_control) {
	ASSERT_FALSE(iscntrl(' '));
	ASSERT_FALSE(iscntrl('a'));
	ASSERT_FALSE(iscntrl('0'));
	ASSERT_FALSE(iscntrl('~'));
}

TEST(iscntrl_boundaries) {
	ASSERT_TRUE(iscntrl(31));
	ASSERT_FALSE(iscntrl(32));
	ASSERT_TRUE(iscntrl(127));
	ASSERT_FALSE(iscntrl(128));
}

TEST(iscntrl_null) {
	ASSERT_TRUE(iscntrl('\0'));
}

TEST(iscntrl_negative) {
	ASSERT_FALSE(iscntrl(-1));
}

TEST(iscntrl_high_ascii) {
	ASSERT_FALSE(iscntrl(128));
	ASSERT_FALSE(iscntrl(255));
}

/* ============================================================================
 * isprint
 * ============================================================================ */
TEST_SUITE(isprint);

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
}

TEST(isprint_boundaries) {
	ASSERT_TRUE(isprint(32));
	ASSERT_FALSE(isprint(31));
	ASSERT_TRUE(isprint(126));
	ASSERT_FALSE(isprint(127));
}

TEST(isprint_null) {
	ASSERT_FALSE(isprint('\0'));
}

TEST(isprint_negative) {
	ASSERT_FALSE(isprint(-1));
}

TEST(isprint_high_ascii) {
	ASSERT_FALSE(isprint(128));
	ASSERT_FALSE(isprint(255));
}

/* ============================================================================
 * isgraph
 * ============================================================================ */
TEST_SUITE(isgraph);

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
}

TEST(isgraph_boundaries) {
	ASSERT_FALSE(isgraph(32));
	ASSERT_TRUE(isgraph(33));
	ASSERT_TRUE(isgraph(126));
	ASSERT_FALSE(isgraph(127));
}

TEST(isgraph_null) {
	ASSERT_FALSE(isgraph('\0'));
}

TEST(isgraph_negative) {
	ASSERT_FALSE(isgraph(-1));
}

TEST(isgraph_high_ascii) {
	ASSERT_FALSE(isgraph(128));
	ASSERT_FALSE(isgraph(255));
}

/* ============================================================================
 * ispunct
 * ============================================================================ */
TEST_SUITE(ispunct);

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

TEST(ispunct_boundaries) {
	ASSERT_TRUE(ispunct('/'));
	ASSERT_FALSE(ispunct('0'));
	ASSERT_TRUE(ispunct(':'));
	ASSERT_TRUE(ispunct('@'));
}

TEST(ispunct_null) {
	ASSERT_FALSE(ispunct('\0'));
}

TEST(ispunct_negative) {
	ASSERT_FALSE(ispunct(-1));
}

TEST(ispunct_high_ascii) {
	ASSERT_FALSE(ispunct(128));
	ASSERT_FALSE(ispunct(255));
}

TEST_MAIN()
