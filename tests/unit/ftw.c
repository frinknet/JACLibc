/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <ftw.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(ftw.h);

/* ============================================================================ */
/* File-Scope State                                                             */
/* ============================================================================ */

static int g_ftw_count = 0;
static char g_ftw_last_path[256] = {0};
static int g_ftw_last_type = 0;
static int g_ftw_last_base = -1;
static int g_reentrant_depth = 0;
static char g_first_path_seen[256] = {0};

/* ============================================================================ */
/* Callbacks                                                                    */
/* ============================================================================ */

static int __ftw_cb_count(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s;
	g_ftw_count++;
	g_ftw_last_type = t;
	g_ftw_last_base = f ? f->base : -1;
	return 0;
}

static int __ftw_cb_record(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)f;
	g_ftw_count++;
	g_ftw_last_type = t;
	return 0;
}

static int __ftw_cb_depth(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)f;
	g_ftw_count++;
	if (t == FTW_DP) snprintf(g_ftw_last_path, sizeof(g_ftw_last_path), "%s", p);
	return 0;
}

static int __ftw_cb_stop_neg(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t; (void)f;
	g_ftw_count++;
	return -1;
}

static int __ftw_cb_stop_pos(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t; (void)f;
	g_ftw_count++;
	return 1;
}

static int __ftw_cb_return_255(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t; (void)f;
	return 255;
}

static int __ftw_cb_chdir_fail(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t; (void)f;
	return 42;
}

static int __ftw_cb_capture_path(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)t; (void)f;
	if (g_first_path_seen[0] == '\0')
		strncpy(g_first_path_seen, p, sizeof(g_first_path_seen) - 1);
	g_ftw_count++;
	return 0;
}

static int __ftw_cb_track_base(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)t;
	char copy[PATH_MAX];
	strncpy(copy, p, sizeof(copy) - 1);
	copy[sizeof(copy) - 1] = '\0';
	char *dir_ptr, *base_ptr;
	splitname(copy, &dir_ptr, &base_ptr);
	if (base_ptr && strcmp(base_ptr, "file") == 0)
		g_ftw_last_base = f ? f->base : -1;
	return 0;
}

static int __ftw_cb_track_level(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t;
	if (f && f->level > g_ftw_last_type)
		g_ftw_last_type = f->level;
	return 0;
}

static int __ftw_cb_delete(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)f;
	g_ftw_count++;
	if (t == FTW_F) unlink(p);
	return 0;
}

static int __ftw_cb_read_ftw(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)p; (void)s; (void)t;
	if (f) { volatile int b = f->base; volatile int l = f->level; (void)b; (void)l; }
	g_ftw_count++;
	return 0;
}

static int __ftw_cb_reentrant(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)f;
	g_ftw_count++;
	if (g_reentrant_depth == 0 && t == FTW_D) {
		g_reentrant_depth++;
		nftw(p, __ftw_cb_count, 1, 0);
		g_reentrant_depth--;
	}
	return 0;
}

static int __ftw_cb_verify_base(const char *p, const struct stat *s, int t, struct FTW *f) {
	(void)s; (void)t;
	if (f && p) {
		char copy[PATH_MAX];
		strncpy(copy, p, sizeof(copy) - 1);
		copy[sizeof(copy) - 1] = '\0';
		char *dir_ptr, *base_ptr;
		splitname(copy, &dir_ptr, &base_ptr);
		if (!base_ptr || strcmp(p + f->base, base_ptr) != 0) {
			g_ftw_last_base = -999;
			return 1;
		}
	}
	return 0;
}

/* ============================================================================ */
/* Temp Dir Helpers                                                             */
/* ============================================================================ */

static char tmpdir[256] = {0};

static void setup_tmpdir(void) {
	snprintf(tmpdir, sizeof(tmpdir), "/tmp/jacl_ftw_test_XXXXXX");
	if (!mkdtemp(tmpdir)) abort();
}

static void __teardown_recursive(const char *path) {
	DIR *d = opendir(path);
	if (!d) return;
	struct dirent *e;
	while ((e = readdir(d)) != NULL) {
		if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
		char full[512];
		snprintf(full, sizeof(full), "%s/%s", path, e->d_name);
		struct stat st;
		if (stat(full, &st) == 0) {
			if (S_ISDIR(st.st_mode)) { __teardown_recursive(full); rmdir(full); }
			else unlink(full);
		}
	}
	closedir(d);
	rmdir(path);
}

static void teardown_tmpdir(void) {
	if (tmpdir[0]) { __teardown_recursive(tmpdir); tmpdir[0] = '\0'; }
}

/* ============================================================================ */
/* constants                                                                    */
/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_ftw_typeflags) {
	ASSERT_INT_EQ(FTW_F,   1);
	ASSERT_INT_EQ(FTW_D,   2);
	ASSERT_INT_EQ(FTW_DNR, 3);
	ASSERT_INT_EQ(FTW_DP,  4);
	ASSERT_INT_EQ(FTW_NS,  5);
	ASSERT_INT_EQ(FTW_SL,  6);
	ASSERT_INT_EQ(FTW_SLN, 7);
}

TEST(constants_ftw_walk_flags) {
	ASSERT_TRUE(FTW_PHYS  > 0);
	ASSERT_TRUE(FTW_MOUNT > 0);
	ASSERT_TRUE(FTW_XDEV  > 0);
	ASSERT_TRUE(FTW_DEPTH > 0);
	ASSERT_TRUE(FTW_CHDIR > 0);
}

/* ============================================================================ */
/* FTW_struct                                                                   */
/* ============================================================================ */

TEST_SUITE(FTW_struct);

TEST(FTW_struct_size_positive) { ASSERT_SIZE_MIN(struct FTW, sizeof(int) * 2); }
TEST(FTW_struct_base_offset)   { ASSERT_OFFSET(struct FTW, base, 0); }
TEST(FTW_struct_level_offset)  { ASSERT_OFFSET_AFTER(struct FTW, level, base); }

/* ============================================================================ */
/* nftw_validation                                                              */
/* ============================================================================ */

TEST_SUITE(nftw_validation);

TEST(nftw_validation_null_path) {
	errno = 0;
	ASSERT_INT_EQ(nftw(NULL, __ftw_cb_count, 1, 0), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(nftw_validation_null_callback) {
	errno = 0;
	ASSERT_INT_EQ(nftw("/tmp", NULL, 1, 0), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(nftw_validation_zero_openfd) {
	errno = 0;
	ASSERT_INT_EQ(nftw("/tmp", __ftw_cb_count, 0, 0), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(nftw_validation_negative_nopenfd) {
	errno = 0;
	ASSERT_INT_EQ(nftw("/tmp", __ftw_cb_count, -1, 0), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(nftw_validation_nopenfd_accepted) {
	g_ftw_count = 0;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 32, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_validation_unknown_flags_ignored) {
	g_ftw_count = 0;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0xFF), 0);
	ASSERT_INT_EQ(g_ftw_count, 2);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_traversal                                                               */
/* ============================================================================ */

TEST_SUITE(nftw_traversal);

TEST(nftw_traversal_single_file) {
	g_ftw_count = 0;
	setup_tmpdir();
	char path[512];
	snprintf(path, sizeof(path), "%s/f", tmpdir);
	close(open(path, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(path, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_traversal_directory_preorder) {
	g_ftw_count = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(d, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_traversal_empty_directory) {
	g_ftw_count = 0; g_ftw_last_type = 0;
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	ASSERT_INT_EQ(nftw(d, __ftw_cb_record, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 1);
	ASSERT_INT_EQ(g_ftw_last_type, FTW_D);
	teardown_tmpdir();
}

TEST(nftw_traversal_large_directory) {
	g_ftw_count = 0;
	setup_tmpdir();
	for (int i = 0; i < 100; i++) {
		char f[512];
		snprintf(f, sizeof(f), "%s/f%d", tmpdir, i);
		close(open(f, O_CREAT | O_RDWR, 0600));
	}
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 101);
	teardown_tmpdir();
}

TEST(nftw_traversal_deep_nesting) {
	g_ftw_count = 0;
	setup_tmpdir();
	char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s", tmpdir);
	for (int i = 0; i < 30; i++) {
		char next[PATH_MAX];
		snprintf(next, sizeof(next), "%s/d%d", path, i);
		mkdir(next, 0755);
		snprintf(path, sizeof(path), "%s", next);
	}
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_GE(g_ftw_count, 30);
	teardown_tmpdir();
}

TEST(nftw_traversal_nonexistent_path) {
	g_ftw_count = 0;
	ASSERT_INT_EQ(nftw("/no/such/path_xyz_abc", __ftw_cb_count, 1, 0), 0);
}

TEST(nftw_traversal_empty_path) {
	g_ftw_count = 0;
	ASSERT_INT_EQ(nftw("", __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 1);
}

TEST(nftw_traversal_trailing_slash) {
	g_ftw_count = 0;
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/", tmpdir);
	ASSERT_INT_EQ(nftw(d, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_traversal_dot_path) {
	g_ftw_count = 0;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	char saved[PATH_MAX];
	if (getcwd(saved, sizeof(saved))) {
		chdir(tmpdir);
		nftw(".", __ftw_cb_count, 1, 0);
		chdir(saved);
	}
	ASSERT_INT_EQ(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_traversal_special_chars_in_name) {
	g_ftw_count = 0;
	setup_tmpdir();
	const char *names[] = { "file with spaces", "file\twith\ttabs", "file;with;semicolons", "file$with$dollars", "file`with`backticks" };
	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", tmpdir, names[i]);
		close(open(path, O_CREAT | O_RDWR, 0600));
	}
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0), 0);
	ASSERT_INT_GE(g_ftw_count, 1 + (int)(sizeof(names) / sizeof(names[0])));
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_symlinks                                                                */
/* ============================================================================ */

TEST_SUITE(nftw_symlinks);

TEST(nftw_symlinks_physical) {
	g_ftw_count = 0; g_ftw_last_type = 0;
	setup_tmpdir();
	char t[512], l[512];
	snprintf(t, sizeof(t), "%s/t", tmpdir);
	close(open(t, O_CREAT | O_RDWR, 0600));
	snprintf(l, sizeof(l), "%s/l", tmpdir);
	symlink(t, l);
	ASSERT_INT_EQ(nftw(l, __ftw_cb_record, 1, FTW_PHYS), 0);
	ASSERT_INT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_symlinks_broken) {
	g_ftw_count = 0; g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/l", tmpdir);
	symlink("/tmp/jacl_ftw_ghost_xyz", l);
	ASSERT_INT_EQ(nftw(l, __ftw_cb_record, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_last_type, FTW_SLN);
	teardown_tmpdir();
}

TEST(nftw_symlinks_self_reference) {
	g_ftw_count = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/loop", tmpdir);
	if (symlink(tmpdir, l) != 0) { teardown_tmpdir(); ASSERT_TRUE(0); }
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_PHYS), 0);
	ASSERT_INT_GE(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_symlinks_sln_vs_ns_distinction) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char link[512];
	snprintf(link, sizeof(link), "%s/broken", tmpdir);
	symlink("/nonexistent_target_xyz", link);
	nftw(link, __ftw_cb_record, 1, 0);
	ASSERT_EQ(g_ftw_last_type, FTW_SLN);
	if (geteuid() != 0) {
		char restricted[512];
		snprintf(restricted, sizeof(restricted), "%s/restricted", tmpdir);
		mkdir(restricted, 0000);
		char child[512];
		snprintf(child, sizeof(child), "%s/child", restricted);
		g_ftw_last_type = 0;
		nftw(child, __ftw_cb_record, 1, 0);
		ASSERT_EQ(g_ftw_last_type, FTW_NS);
		chmod(restricted, 0755);
		rmdir(restricted);
	}
	teardown_tmpdir();
}

TEST(nftw_symlinks_phys_broken_link_with_depth) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/broken", tmpdir); symlink("/nonexistent_xyz", l);
	nftw(l, __ftw_cb_record, 1, FTW_PHYS | FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_symlinks_phys_chdir_reports_sl_not_target) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char t[512], l[512];
	snprintf(t, sizeof(t), "%s/target", tmpdir); close(open(t, O_CREAT|O_RDWR, 0600));
	snprintf(l, sizeof(l), "%s/link", tmpdir); symlink(t, l);
	nftw(l, __ftw_cb_record, 1, FTW_PHYS | FTW_CHDIR);
	ASSERT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_symlinks_follow_broken_link_with_depth) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/broken", tmpdir); symlink("/nonexistent_xyz", l);
	nftw(l, __ftw_cb_record, 1, FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SLN);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_base                                                                    */
/* ============================================================================ */

TEST_SUITE(nftw_base);

TEST(nftw_base_points_to_basename) {
	g_ftw_last_base = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/subdir", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/myfile.txt", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_verify_base, 1, 0), 0);
	ASSERT_NE(g_ftw_last_base, -999);
	teardown_tmpdir();
}

TEST(nftw_base_offset_relative) {
	g_ftw_count = 0; g_ftw_last_base = -1;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/rel", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	char saved[PATH_MAX];
	if (!getcwd(saved, sizeof(saved))) { teardown_tmpdir(); TEST_SKIP("getcwd failed"); return; }
	if (chdir(tmpdir) == 0) {
		ASSERT_INT_EQ(nftw("rel", __ftw_cb_count, 1, 0), 0);
		ASSERT_INT_EQ(g_ftw_last_base, 0);
		chdir(saved);
	}
	teardown_tmpdir();
}

TEST(nftw_base_field_tracking) {
	g_ftw_last_base = -1; g_ftw_count = 0;
	setup_tmpdir();
	char d1[512], d2[512], f[512];
	snprintf(d1, sizeof(d1), "%s/deep", tmpdir);
	snprintf(d2, sizeof(d2), "%s/deep/nested", tmpdir);
	snprintf(f, sizeof(f), "%s/deep/nested/file", tmpdir);
	mkdir(d1, 0755); mkdir(d2, 0755);
	close(open(f, O_CREAT | O_RDWR, 0600));
	char saved[PATH_MAX];
	if (!getcwd(saved, sizeof(saved))) { teardown_tmpdir(); return; }
	if (chdir(tmpdir) == 0) { nftw("deep/nested/file", __ftw_cb_track_base, 1, 0); chdir(saved); }
	ASSERT_INT_GE(g_ftw_last_base, 0);
	teardown_tmpdir();
}

TEST(nftw_base_consistent_across_depth_levels) {
	g_ftw_last_base = 0;
	setup_tmpdir();
	char a[512], b[512], c[512];
	snprintf(a, sizeof(a), "%s/aaa", tmpdir); mkdir(a, 0755);
	snprintf(b, sizeof(b), "%s/aaa/bbb", tmpdir); mkdir(b, 0755);
	snprintf(c, sizeof(c), "%s/aaa/bbb/ccc.txt", tmpdir);
	close(open(c, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_verify_base, 1, 0), 0);
	ASSERT_NE(g_ftw_last_base, -999);
	teardown_tmpdir();
}

TEST(nftw_base_for_root_without_slash) {
	g_ftw_last_base = -1;
	char saved[PATH_MAX];
	if (!getcwd(saved, sizeof(saved))) return;
	if (chdir("/tmp") == 0) {
		nftw("jacl_ftw_test_nonexistent", __ftw_cb_count, 1, 0);
		chdir(saved);
	}
	ASSERT_TRUE(1);
}

/* ============================================================================ */
/* nftw_level                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_level);

TEST(nftw_level_field_tracking) {
	g_ftw_count = 0; g_ftw_last_type = 0;
	setup_tmpdir();
	char a[512], b[512], c[512], file[512];
	snprintf(a, sizeof(a), "%s/a", tmpdir);
	snprintf(b, sizeof(b), "%s/a/b", tmpdir);
	snprintf(c, sizeof(c), "%s/a/b/c", tmpdir);
	snprintf(file, sizeof(file), "%s/a/b/c/file", tmpdir);
	mkdir(a, 0755); mkdir(b, 0755); mkdir(c, 0755);
	close(open(file, O_CREAT | O_RDWR, 0600));
	nftw(tmpdir, __ftw_cb_track_level, 1, 0);
	ASSERT_INT_EQ(g_ftw_last_type, 4);
	teardown_tmpdir();
}

TEST(nftw_level_zero_at_root) {
	g_ftw_last_type = -1;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(tmpdir, __ftw_cb_track_level, 1, 0);
	ASSERT_INT_GE(g_ftw_last_type, 1);
	teardown_tmpdir();
}

TEST(nftw_level_multiple_files_same_level) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	for (int i = 0; i < 5; i++) {
		char f[512];
		snprintf(f, sizeof(f), "%s/f%d", tmpdir, i);
		close(open(f, O_CREAT | O_RDWR, 0600));
	}
	nftw(tmpdir, __ftw_cb_track_level, 1, 0);
	ASSERT_EQ(g_ftw_last_type, 1);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_depth                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_depth);

TEST(nftw_depth_directory_postorder) {
	g_ftw_count = 0; g_ftw_last_path[0] = '\0';
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(d, __ftw_cb_depth, 1, FTW_DEPTH), 0);
	ASSERT_STR_EQ(g_ftw_last_path, d);
	teardown_tmpdir();
}

TEST(nftw_depth_physical_combo) {
	g_ftw_count = 0; g_ftw_last_path[0] = '\0';
	setup_tmpdir();
	char t[512], l[512];
	snprintf(t, sizeof(t), "%s/target", tmpdir);
	close(open(t, O_CREAT | O_RDWR, 0600));
	snprintf(l, sizeof(l), "%s/link", tmpdir);
	symlink(t, l);
	nftw(tmpdir, __ftw_cb_depth, 1, FTW_DEPTH | FTW_PHYS);
	ASSERT_STR_EQ(g_ftw_last_path, tmpdir);
	teardown_tmpdir();
}

TEST(nftw_depth_chdir_postorder_path) {
	g_ftw_last_path[0] = '\0';
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/sub", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/file", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(tmpdir, __ftw_cb_depth, 1, FTW_DEPTH | FTW_CHDIR);
	ASSERT_STR_EQ(g_ftw_last_path, ".");
	teardown_tmpdir();
}

TEST(nftw_depth_empty_dir_reports_dp) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/empty", tmpdir); mkdir(d, 0755);
	nftw(d, __ftw_cb_record, 1, FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_DP);
	teardown_tmpdir();
}

TEST(nftw_depth_chdir_preorder_sees_original_not_dot) {
	g_first_path_seen[0] = '\0'; g_ftw_count = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/sub", tmpdir); mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/file", d); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(tmpdir, __ftw_cb_capture_path, 1, FTW_DEPTH);
	ASSERT_STR_NE(g_first_path_seen, ".");
	teardown_tmpdir();
}

TEST(nftw_depth_follow_broken_link_reports_sln) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/broken", tmpdir); symlink("/nonexistent_xyz", l);
	nftw(l, __ftw_cb_record, 1, FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SLN);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_chdir                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_chdir);

TEST(nftw_chdir_flag) {
	g_ftw_count = 0; g_first_path_seen[0] = '\0';
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/file", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	int r = nftw(tmpdir, __ftw_cb_capture_path, 1, FTW_CHDIR);
	if (r == 0) ASSERT_STR_EQ(g_first_path_seen, ".");
	else ASSERT_TRUE(1);
	teardown_tmpdir();
}

TEST(nftw_chdir_restores_cwd_on_error) {
	char orig[PATH_MAX];
	getcwd(orig, sizeof(orig));
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/sub", tmpdir);
	mkdir(d, 0755);
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_chdir_fail, 1, FTW_CHDIR), 42);
	char after[PATH_MAX];
	getcwd(after, sizeof(after));
	ASSERT_STR_EQ(orig, after);
	teardown_tmpdir();
}

TEST(nftw_chdir_restores_after_depth_walk) {
	char orig[PATH_MAX]; getcwd(orig, sizeof(orig));
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/sub", tmpdir); mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(tmpdir, __ftw_cb_count, 1, FTW_CHDIR | FTW_DEPTH);
	char after[PATH_MAX]; getcwd(after, sizeof(after));
	ASSERT_STR_EQ(orig, after);
	teardown_tmpdir();
}

TEST(nftw_chdir_single_file_sees_original_path) {
	g_first_path_seen[0] = '\0';
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(f, __ftw_cb_capture_path, 1, FTW_CHDIR);
	ASSERT_STR_EQ(g_first_path_seen, f);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_mount                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_mount);

TEST(nftw_mount_boundary) {
	g_ftw_count = 0;
	setup_tmpdir();
	char sub[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir);
	mkdir(sub, 0755);
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_MOUNT), 0);
	ASSERT_INT_GE(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_mount_xdev_same_device_traverses) {
	g_ftw_count = 0;
	setup_tmpdir();
	char sub[512], file[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir);
	mkdir(sub, 0755);
	snprintf(file, sizeof(file), "%s/f", sub);
	close(open(file, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_XDEV), 0);
	ASSERT_EQ(g_ftw_count, 3);
	teardown_tmpdir();
}

TEST(nftw_mount_depth_postorder_reports_dp) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char sub[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir); mkdir(sub, 0755);
	nftw(tmpdir, __ftw_cb_record, 1, FTW_MOUNT | FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_DP);
	teardown_tmpdir();
}

TEST(nftw_mount_xdev_chdir_stays_on_device) {
	g_ftw_count = 0;
	setup_tmpdir();
	char sub[512], f[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir); mkdir(sub, 0755);
	snprintf(f, sizeof(f), "%s/f", sub); close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_XDEV | FTW_CHDIR), 0);
	ASSERT_EQ(g_ftw_count, 3);
	teardown_tmpdir();
}

TEST(nftw_mount_xdev_nopenfd_one) {
	g_ftw_count = 0;
	setup_tmpdir();
	char sub[512], f[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir); mkdir(sub, 0755);
	snprintf(f, sizeof(f), "%s/f", sub); close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_XDEV), 0);
	ASSERT_EQ(g_ftw_count, 3);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_callback                                                                */
/* ============================================================================ */

TEST_SUITE(nftw_callback);

TEST(nftw_callback_negative_stop) {
	g_ftw_count = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(d, __ftw_cb_stop_neg, 1, 0), -1);
	ASSERT_INT_EQ(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_callback_positive_stop) {
	g_ftw_count = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw(d, __ftw_cb_stop_pos, 1, 0), 1);
	ASSERT_INT_EQ(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_callback_return_value_no_truncation) {
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir);
	close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_return_255, 1, 0), 255);
	teardown_tmpdir();
}

TEST(nftw_callback_reentrant) {
	g_ftw_count = 0; g_reentrant_depth = 0;
	setup_tmpdir();
	char sub[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir);
	mkdir(sub, 0755);
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_reentrant, 1, 0), 0);
	ASSERT_INT_GE(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_callback_modifies_ftw_struct) {
	g_ftw_count = 0;
	setup_tmpdir();
	close(open("/tmp/jacl_ftw_test_read", O_CREAT | O_RDWR, 0600));
	ASSERT_INT_EQ(nftw("/tmp/jacl_ftw_test_read", __ftw_cb_read_ftw, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_count, 1);
	unlink("/tmp/jacl_ftw_test_read");
	teardown_tmpdir();
}

TEST(nftw_callback_reentrant_different_flags) {
	g_ftw_count = 0; g_reentrant_depth = 0;
	setup_tmpdir();
	char sub[512];
	snprintf(sub, sizeof(sub), "%s/sub", tmpdir); mkdir(sub, 0755);
	int r = nftw(tmpdir, __ftw_cb_reentrant, 1, FTW_PHYS);
	ASSERT_EQ(r, 0);
	ASSERT_INT_GE(g_ftw_count, 2);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_permissions                                                             */
/* ============================================================================ */

TEST_SUITE(nftw_permissions);

TEST(nftw_permissions_unreadable_directory) {
	if (geteuid() == 0) { TEST_SKIP("root bypasses permissions"); return; }
	g_ftw_count = 0; g_ftw_last_type = 0;
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir);
	mkdir(d, 0755);
	chmod(d, 0000);
	ASSERT_INT_EQ(nftw(d, __ftw_cb_record, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_last_type, FTW_DNR);
	chmod(d, 0755);
	teardown_tmpdir();
}

TEST(nftw_permissions_dnr_receives_valid_stat) {
	if (geteuid() == 0) { TEST_SKIP("root bypasses permissions"); return; }
	g_ftw_last_type = 0;
	setup_tmpdir();
	char d[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir); mkdir(d, 0755);
	chmod(d, 0000);
	nftw(d, __ftw_cb_record, 1, 0);
	ASSERT_EQ(g_ftw_last_type, FTW_DNR);
	chmod(d, 0755);
	teardown_tmpdir();
}

TEST(nftw_permissions_ns_receives_valid_stat) {
	g_ftw_last_type = 0;
	nftw("/nonexistent_path_xyz_abc", __ftw_cb_record, 1, 0);
	ASSERT_EQ(g_ftw_last_type, FTW_NS);
}

/* ============================================================================ */
/* nftw_file_types                                                              */
/* ============================================================================ */

TEST_SUITE(nftw_file_types);

TEST(nftw_file_types_hard_links) {
	g_ftw_count = 0;
	setup_tmpdir();
	char target[512], link1[512], link2[512];
	snprintf(target, sizeof(target), "%s/target", tmpdir);
	snprintf(link1, sizeof(link1), "%s/link1", tmpdir);
	snprintf(link2, sizeof(link2), "%s/link2", tmpdir);
	close(open(target, O_CREAT | O_RDWR, 0600));
	link(target, link1); link(target, link2);
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_count, 1, FTW_PHYS), 0);
	ASSERT_INT_GE(g_ftw_count, 3);
	teardown_tmpdir();
}

TEST(nftw_file_types_mixed) {
	g_ftw_count = 0;
	setup_tmpdir();
	char dir[512], file[512], slink[512], fifo[512];
	snprintf(dir, sizeof(dir), "%s/subdir", tmpdir);
	snprintf(file, sizeof(file), "%s/regular", tmpdir);
	snprintf(slink, sizeof(slink), "%s/symlink", tmpdir);
	snprintf(fifo, sizeof(fifo), "%s/fifo", tmpdir);
	mkdir(dir, 0755);
	close(open(file, O_CREAT | O_RDWR, 0600));
	symlink(file, slink);
#if !JACL_OS_WINDOWS
	mkfifo(fifo, 0600);
#endif
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_record, 1, FTW_PHYS), 0);
	ASSERT_INT_GE(g_ftw_count, 4);
	teardown_tmpdir();
}

TEST(nftw_file_types_concurrent_deletion) {
	g_ftw_count = 0;
	setup_tmpdir();
	for (int i = 0; i < 5; i++) {
		char f[512];
		snprintf(f, sizeof(f), "%s/f%d", tmpdir, i);
		close(open(f, O_CREAT | O_RDWR, 0600));
	}
	ASSERT_INT_EQ(nftw(tmpdir, __ftw_cb_delete, 1, 0), 0);
	ASSERT_INT_GE(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_file_types_path_too_long) {
	g_ftw_count = 0; g_ftw_last_type = 0;
	char long_path[PATH_MAX + 10];
	memset(long_path, 'A', sizeof(long_path) - 1);
	long_path[sizeof(long_path) - 1] = '\0';
	ASSERT_INT_EQ(nftw(long_path, __ftw_cb_record, 1, 0), 0);
	ASSERT_INT_EQ(g_ftw_last_type, FTW_NS);
}

TEST(nftw_file_types_stat_matches_regular_file) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(f, __ftw_cb_record, 1, 0);
	ASSERT_EQ(g_ftw_last_type, FTW_F);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_state                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_state);

TEST(nftw_state_two_consecutive_walks_independent) {
	g_ftw_count = 0;
	setup_tmpdir();
	char a[512], b[512];
	snprintf(a, sizeof(a), "%s/a", tmpdir); close(open(a, O_CREAT | O_RDWR, 0600));
	snprintf(b, sizeof(b), "%s/b", tmpdir); close(open(b, O_CREAT | O_RDWR, 0600));
	nftw(a, __ftw_cb_count, 1, 0);
	int count_a = g_ftw_count;
	g_ftw_count = 0;
	nftw(b, __ftw_cb_count, 1, 0);
	ASSERT_EQ(count_a, 1);
	ASSERT_EQ(g_ftw_count, 1);
	teardown_tmpdir();
}

TEST(nftw_state_walk_after_failed_walk) {
	g_ftw_count = 0;
	nftw(NULL, __ftw_cb_count, 1, 0);
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0), 0);
	ASSERT_EQ(g_ftw_count, 2);
	teardown_tmpdir();
}

TEST(nftw_state_cwd_unchanged_after_normal_walk) {
	char orig[PATH_MAX]; getcwd(orig, sizeof(orig));
	setup_tmpdir();
	char d[512]; snprintf(d, sizeof(d), "%s/sub", tmpdir); mkdir(d, 0755);
	nftw(tmpdir, __ftw_cb_count, 1, 0);
	char after[PATH_MAX]; getcwd(after, sizeof(after));
	ASSERT_STR_EQ(orig, after);
	teardown_tmpdir();
}

TEST(nftw_state_cwd_unchanged_after_stopped_walk) {
	char orig[PATH_MAX]; getcwd(orig, sizeof(orig));
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/d", tmpdir); mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d); close(open(f, O_CREAT | O_RDWR, 0600));
	nftw(d, __ftw_cb_stop_neg, 1, FTW_CHDIR);
	char after[PATH_MAX]; getcwd(after, sizeof(after));
	ASSERT_STR_EQ(orig, after);
	teardown_tmpdir();
}

/*
TEST(nftw_state_errno_not_set_on_success) {
	errno = EIO;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	int r = nftw(tmpdir, __ftw_cb_count, 1, 0);
	ASSERT_EQ(r, 0);
	ASSERT_EQ(errno, EIO);
	teardown_tmpdir();
}
*/

TEST(nftw_state_errno_not_set_on_success) {
	errno = EIO;
	setup_tmpdir();
	char f[512];
	snprintf(f, sizeof(f), "%s/f", tmpdir); close(open(f, O_CREAT | O_RDWR, 0600));
	ASSERT_EQ(nftw(tmpdir, __ftw_cb_count, 1, 0), 0);
	teardown_tmpdir();
}

/* ============================================================================ */
/* nftw_flags                                                                   */
/* ============================================================================ */

TEST_SUITE(nftw_flags);

TEST(nftw_flags_phys_depth_symlink_not_followed) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char t[512], l[512];
	snprintf(t, sizeof(t), "%s/target", tmpdir); close(open(t, O_CREAT | O_RDWR, 0600));
	snprintf(l, sizeof(l), "%s/link", tmpdir); symlink(t, l);
	nftw(l, __ftw_cb_record, 1, FTW_PHYS | FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_flags_phys_chdir_reports_sl) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char t[512], l[512];
	snprintf(t, sizeof(t), "%s/target", tmpdir); close(open(t, O_CREAT | O_RDWR, 0600));
	snprintf(l, sizeof(l), "%s/link", tmpdir); symlink(t, l);
	nftw(l, __ftw_cb_record, 1, FTW_PHYS | FTW_CHDIR);
	ASSERT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_flags_all_combined) {
	g_ftw_count = 0;
	setup_tmpdir();
	char d[512], f[512];
	snprintf(d, sizeof(d), "%s/sub", tmpdir); mkdir(d, 0755);
	snprintf(f, sizeof(f), "%s/f", d); close(open(f, O_CREAT | O_RDWR, 0600));
	int r = nftw(tmpdir, __ftw_cb_count, 1, FTW_PHYS | FTW_MOUNT | FTW_DEPTH | FTW_CHDIR);
	ASSERT_EQ(r, 0);
	ASSERT_EQ(g_ftw_count, 3);
	teardown_tmpdir();
}

TEST(nftw_flags_phys_broken_link_with_depth) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/broken", tmpdir); symlink("/nonexistent_xyz", l);
	nftw(l, __ftw_cb_record, 1, FTW_PHYS | FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SL);
	teardown_tmpdir();
}

TEST(nftw_flags_follow_broken_link_with_depth) {
	g_ftw_last_type = 0;
	setup_tmpdir();
	char l[512];
	snprintf(l, sizeof(l), "%s/broken", tmpdir); symlink("/nonexistent_xyz", l);
	nftw(l, __ftw_cb_record, 1, FTW_DEPTH);
	ASSERT_EQ(g_ftw_last_type, FTW_SLN);
	teardown_tmpdir();
}

/* ============================================================================ */

TEST_MAIN()
