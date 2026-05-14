/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <libgen.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(libgen.h);

static void test_split(const char *input, const char *exp_dir, const char *exp_base) {
	char p[256];
	strncpy(p, input, sizeof(p) - 1);
	p[sizeof(p) - 1] = '\0';

	char *d, *b;
	splitname(p, &d, &b);

	ASSERT_STR_EQ(d, exp_dir);
	ASSERT_STR_EQ(b, exp_base);
}

/* ============================================================================ */
TEST_SUITE(libgen_splitname);

TEST(splitname_normal)             { test_split("usr/lib", "usr", "lib"); }
TEST(splitname_trailing_slash)     { test_split("usr/", ".", "usr"); }
TEST(splitname_no_slash)           { test_split("usr", ".", "usr"); }
TEST(splitname_root)               { test_split("/", "/", "/"); }
TEST(splitname_dot)                { test_split(".", ".", "."); }
TEST(splitname_double_slash)       { test_split("/usr//lib/", "/usr", "lib"); }
TEST(splitname_triple_slash)       { test_split("///", "/", "/"); }
TEST(splitname_dot_prefix)         { test_split("./file", ".", "file"); }
TEST(splitname_deep_path)          { test_split("a/b/c/d/e", "a/b/c/d", "e"); }
TEST(splitname_absolute)           { test_split("/usr/bin", "/usr", "bin"); }

/* ============================================================================ */
TEST_SUITE(libgen_basename);

TEST(basename_normal)          { char p[]="usr/lib";      char *r = basename(p);      ASSERT_STR_EQ(r,"lib"); }
TEST(basename_trailing_slash)  { char p[]="usr/";         char *r = basename(p);      ASSERT_STR_EQ(r,"usr"); }
TEST(basename_no_slash)        { char p[]="usr";          char *r = basename(p);      ASSERT_STR_EQ(r,"usr"); }
TEST(basename_root)            { char p[]="/";            char *r = basename(p);      ASSERT_STR_EQ(r,"/"); }
TEST(basename_dot)             { char p[]=".";            char *r = basename(p);      ASSERT_STR_EQ(r,"."); }
TEST(basename_double_slash)    { char p[]="/usr//lib/";   char *r = basename(p);      ASSERT_STR_EQ(r,"lib"); }
TEST(basename_triple_slash)    { char p[]="///";          char *r = basename(p);      ASSERT_STR_EQ(r,"/"); }
TEST(basename_dot_prefix)      { char p[]="./file";       char *r = basename(p);      ASSERT_STR_EQ(r,"file"); }

/* ============================================================================ */
TEST_SUITE(libgen_dirname);

TEST(dirname_normal)           { char p[]="usr/lib";      char *r = dirname(p);       ASSERT_STR_EQ(r,"usr"); }
TEST(dirname_trailing_slash)   { char p[]="usr/";         char *r = dirname(p);       ASSERT_STR_EQ(r,"."); }
TEST(dirname_no_slash)         { char p[]="usr";          char *r = dirname(p);       ASSERT_STR_EQ(r,"."); }
TEST(dirname_root)             { char p[]="/";            char *r = dirname(p);       ASSERT_STR_EQ(r,"/"); }
TEST(dirname_dot)              { char p[]=".";            char *r = dirname(p);       ASSERT_STR_EQ(r,"."); }
TEST(dirname_modifies_input)   { char p[]="a/b";          dirname(p);                 ASSERT_STR_EQ(p,"a"); }
TEST(dirname_triple_slash)     { char p[]="///";          char *r = dirname(p);       ASSERT_STR_EQ(r,"/"); }
TEST(dirname_dot_prefix)       { char p[]="./file";       char *r = dirname(p);       ASSERT_STR_EQ(r,"."); }
TEST(dirname_deep_path)        { char p[]="a/b/c/d/e";    char *r = dirname(p);       ASSERT_STR_EQ(r,"a/b/c/d"); }

/* ============================================================================ */
TEST_SUITE(libgen_edge);

TEST(edge_null_basename)       { ASSERT_STR_EQ(basename(NULL), "."); }
TEST(edge_null_dirname)        { ASSERT_STR_EQ(dirname(NULL), "."); }
TEST(edge_empty_basename)      { char p[]="";             char *r = basename(p);      ASSERT_STR_EQ(r, "."); }
TEST(edge_empty_dirname)       { char p[]="";             char *r = dirname(p);       ASSERT_STR_EQ(r, "."); }

/* ============================================================================ */
TEST_MAIN()
