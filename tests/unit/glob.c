/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <glob.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

TEST_TYPE(unit);
TEST_UNIT(glob.h);

#define __TEST_DIR "/tmp/jacl_glob_test"

static void __glob_create(const char *name) {
	char path[512];
	snprintf(path, sizeof(path), "%s/%s", __TEST_DIR, name);
	int fd = open(path, O_CREAT | O_WRONLY, 0644);
	if (fd >= 0) close(fd);
}

static void __glob_path(char *buf, size_t sz, const char *name) {
	snprintf(buf, sz, "%s/%s", __TEST_DIR, name);
}

static void __glob_wipe(void) {
	DIR *d = opendir(__TEST_DIR);
	if (!d) return;
	struct dirent *e;
	while ((e = readdir(d)) != NULL) {
		if (e->d_name[0] == '.') {
			if (e->d_name[1] == '\0') continue; // .
			if (e->d_name[1] == '.' && e->d_name[2] == '\0') continue; // ..
		}
		char p[512]; snprintf(p, sizeof(p), "%s/%s", __TEST_DIR, e->d_name);
		unlink(p);
	}
	closedir(d);
}

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_flags) {
	ASSERT_EQ(0x01, GLOB_ERR);
	ASSERT_EQ(0x02, GLOB_MARK);
	ASSERT_EQ(0x04, GLOB_NOSORT);
	ASSERT_EQ(0x08, GLOB_DOOFFS);
	ASSERT_EQ(0x10, GLOB_NOESCAPE);
	ASSERT_EQ(0x20, GLOB_PERIOD);
}

TEST(constants_returns) {
	ASSERT_EQ(1, GLOB_ABORTED);
	ASSERT_EQ(2, GLOB_NOMATCH);
	ASSERT_EQ(3, GLOB_NOSPACE);
}

/* ============================================================================ */
TEST_SUITE(glob_basic);

TEST(glob_basic_setup) { mkdir(__TEST_DIR, 0755); }

TEST(glob_basic_no_match) {
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob("/tmp/nonexistent_xyz", 0, NULL, &g));
	ASSERT_EQ(0, g.gl_pathc);
	globfree(&g);
}

TEST(glob_basic_literal_file) {
	__glob_create("b_lit.txt");
	char p[512]; __glob_path(p, sizeof(p), "b_lit.txt");
	glob_t g = {0};
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(1, g.gl_pathc);
	globfree(&g);
}

TEST(glob_basic_wildcard_star) {
	__glob_create("b_star_a.txt"); __glob_create("b_star_b.txt");
	char p[512]; __glob_path(p, sizeof(p), "b_star_*.txt");
	glob_t g = {0};
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(2, g.gl_pathc);
	globfree(&g);
}

TEST(glob_basic_wildcard_question) {
	__glob_create("b_q1.txt"); __glob_create("b_q2.txt");
	char p[512]; __glob_path(p, sizeof(p), "b_q?.txt");
	glob_t g = {0};
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(2, g.gl_pathc);
	globfree(&g);
}

TEST(glob_basic_trailing_slash_nomatch) {
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob("/tmp/jacl_nonexistent/", 0, NULL, &g));
	globfree(&g);
}

TEST(glob_basic_empty_directory) {
	char d[512]; snprintf(d, sizeof(d), "%s/b_empty", __TEST_DIR);
	mkdir(d, 0755);
	glob_t g = {0};
	char p[512]; snprintf(p, sizeof(p), "%s/*", d);
	ASSERT_EQ(GLOB_NOMATCH, glob(p, 0, NULL, &g));
	globfree(&g);
	rmdir(d);
}

/* ============================================================================ */
TEST_SUITE(glob_flags);

TEST(glob_flags_mark_directory) {
	char d[512]; snprintf(d, sizeof(d), "%s/f_dir", __TEST_DIR);
	mkdir(d, 0755);
	glob_t g = {0};
	char p[512]; __glob_path(p, sizeof(p), "f_dir");
	ASSERT_EQ(0, glob(p, GLOB_MARK, NULL, &g));
	ASSERT_STR_SUF("/", g.gl_pathv[g.gl_offs]);
	globfree(&g);
	rmdir(d);
}

TEST(glob_flags_mark_file_no_trail) {
	__glob_create("f_mark.txt");
	glob_t g = {0};
	char p[512]; __glob_path(p, sizeof(p), "f_mark.txt");
	ASSERT_EQ(0, glob(p, GLOB_MARK, NULL, &g));
	size_t len = strlen(g.gl_pathv[g.gl_offs]);
	ASSERT_TRUE(len > 0 && g.gl_pathv[g.gl_offs][len - 1] != '/');
	globfree(&g);
}

TEST(glob_flags_dooffs_slots) {
	__glob_create("f_off.txt");
	glob_t g = {0};
	g.gl_offs = 3;
	char p[512]; __glob_path(p, sizeof(p), "f_off.txt");
	ASSERT_EQ(0, glob(p, GLOB_DOOFFS, NULL, &g));
	ASSERT_NULL(g.gl_pathv[0]); ASSERT_NULL(g.gl_pathv[1]); ASSERT_NULL(g.gl_pathv[2]);
	ASSERT_NOT_NULL(g.gl_pathv[3]);
	globfree(&g);
}

TEST(glob_flags_period_wildcard_hidden) {
	__glob_wipe(); // Clear cross-test pollution
	__glob_create(".hidden");
	glob_t g = {0};
	char p[512]; __glob_path(p, sizeof(p), "*");

	// Default: * does NOT match dotfiles (FNM_PERIOD active)
	ASSERT_EQ(GLOB_NOMATCH, glob(p, 0, NULL, &g));

	// GLOB_PERIOD: * DOES match dotfiles (FNM_PERIOD removed)
	memset(&g, 0, sizeof(g));
	ASSERT_EQ(0, glob(p, GLOB_PERIOD, NULL, &g));
	ASSERT_EQ(1, g.gl_pathc);
	globfree(&g);
}

TEST(glob_flags_noescape_literal) {
	__glob_create("f_esc.txt");
	glob_t g = {0};
	/* \f_esc.* with NOESCAPE treats \ as literal -> fails to match f_esc.txt */
	char p[512]; snprintf(p, sizeof(p), "%s/\\f_esc.*", __TEST_DIR);
	ASSERT_EQ(GLOB_NOMATCH, glob(p, GLOB_NOESCAPE, NULL, &g));
	globfree(&g);

	/* Default: \ escapes nothing special, fnmatch unescapes to f_esc.* -> matches */
	memset(&g, 0, sizeof(g));
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(1, g.gl_pathc);
	globfree(&g);
}

TEST(glob_flags_nosort_verify) {
	__glob_create("f_sort_z.txt"); __glob_create("f_sort_a.txt"); __glob_create("f_sort_m.txt");
	char p[512]; __glob_path(p, sizeof(p), "f_sort_*.txt");
	glob_t g = {0};
	ASSERT_EQ(0, glob(p, GLOB_NOSORT, NULL, &g));
	ASSERT_EQ(3, g.gl_pathc);
	globfree(&g);
}

/* ============================================================================ */
TEST_SUITE(glob_errors);

TEST(glob_errors_null_pattern) {
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob(NULL, 0, NULL, &g));
	globfree(&g);
}

TEST(glob_errors_null_pglob) {
	ASSERT_EQ(GLOB_NOMATCH, glob("*", 0, NULL, NULL));
}

TEST(glob_errors_empty_pattern) {
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob("", 0, NULL, &g));
	globfree(&g);
}

TEST(glob_errors_missing_directory) {
	char p[512]; snprintf(p, sizeof(p), "%s/bad_dir/*", __TEST_DIR);
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob(p, 0, NULL, &g));
	globfree(&g);
}

static int __glob_err_cb_ret = 0;
static int __glob_err_cb_count = 0;
static int __glob_err_callback(const char *path, int eerrno) {
	(void)path; (void)eerrno; __glob_err_cb_count++;
	return __glob_err_cb_ret;
}

TEST(glob_errors_callback_continue) {
	__glob_err_cb_ret = 0; __glob_err_cb_count = 0;
	char p[512]; snprintf(p, sizeof(p), "%s/bad_dir/*", __TEST_DIR);
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob(p, GLOB_ERR, __glob_err_callback, &g));
	ASSERT_EQ(1, __glob_err_cb_count);
	globfree(&g);
}

TEST(glob_errors_callback_abort) {
	__glob_err_cb_ret = 1; __glob_err_cb_count = 0;
	char p[512]; snprintf(p, sizeof(p), "%s/bad_dir/*", __TEST_DIR);
	glob_t g = {0};
	ASSERT_EQ(GLOB_ABORTED, glob(p, GLOB_ERR, __glob_err_callback, &g));
	ASSERT_EQ(1, __glob_err_cb_count);
	globfree(&g);
}

TEST(glob_errors_null_errfunc) {
	char p[512]; snprintf(p, sizeof(p), "%s/bad_dir/*", __TEST_DIR);
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob(p, 0, NULL, &g));
	globfree(&g);
}

TEST(glob_errors_invalid_bracket) {
	char p[512]; snprintf(p, sizeof(p), "%s/e_[a-", __TEST_DIR);
	glob_t g = {0};
	ASSERT_EQ(GLOB_NOMATCH, glob(p, 0, NULL, &g));
	globfree(&g);
}

/* ============================================================================ */
TEST_SUITE(glob_memory);

TEST(glob_memory_free_null) {
	globfree(NULL);
	ASSERT_TRUE(1);
}

TEST(glob_memory_free_partial) {
	glob_t g = {0};
	g.gl_pathv = NULL;
	globfree(&g);
	ASSERT_TRUE(1);
}

TEST(glob_memory_free_twice_safety) {
	__glob_create("m_safe.txt");
	char p[512]; __glob_path(p, sizeof(p), "m_safe.txt");
	glob_t g = {0};
	glob(p, 0, NULL, &g);
	globfree(&g);
	globfree(&g);
	ASSERT_TRUE(1);
}

TEST(glob_memory_reuse_clear) {
	__glob_create("m_r1.txt");
	char p[512]; __glob_path(p, sizeof(p), "m_r*.txt");
	glob_t g = {0};
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(1, g.gl_pathc);

	/* Simulate consumer clearing before reuse */
	memset(&g, 0, sizeof(g));
	__glob_create("m_r2.txt");
	ASSERT_EQ(0, glob(p, 0, NULL, &g));
	ASSERT_EQ(2, g.gl_pathc);
	globfree(&g);
}

TEST(glob_memory_cleanup) {
	__glob_wipe();
	rmdir(__TEST_DIR);
	ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_MAIN()
