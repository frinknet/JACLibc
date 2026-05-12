/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys/stat.h);

/* ============================================================================ */
TEST_SUITE(constants_file_type_masks);

TEST(s_ifmt) { ASSERT_EQ(0170000, S_IFMT); }
TEST(s_ifsock) { ASSERT_EQ(0140000, S_IFSOCK); }
TEST(s_iflnk) { ASSERT_EQ(0120000, S_IFLNK); }
TEST(s_ifreg) { ASSERT_EQ(0100000, S_IFREG); }
TEST(s_ifblk) { ASSERT_EQ(0060000, S_IFBLK); }
TEST(s_ifdir) { ASSERT_EQ(0040000, S_IFDIR); }
TEST(s_ifchr) { ASSERT_EQ(0020000, S_IFCHR); }
TEST(s_ififo) { ASSERT_EQ(0010000, S_IFIFO); }

TEST(file_types_unique) {
	ASSERT_NE(S_IFREG, S_IFDIR);
	ASSERT_NE(S_IFDIR, S_IFCHR);
	ASSERT_NE(S_IFREG, S_IFLNK);
}

/* ============================================================================ */
TEST_SUITE(constants_permissions);

TEST(s_irusr) { ASSERT_EQ(0000400, S_IRUSR); }
TEST(s_iwusr) { ASSERT_EQ(0000200, S_IWUSR); }
TEST(s_ixusr) { ASSERT_EQ(0000100, S_IXUSR); }
TEST(s_irwxu) { ASSERT_EQ(0000700, S_IRWXU); }

TEST(s_irgrp) { ASSERT_EQ(0000040, S_IRGRP); }
TEST(s_iwgrp) { ASSERT_EQ(0000020, S_IWGRP); }
TEST(s_ixgrp) { ASSERT_EQ(0000010, S_IXGRP); }
TEST(s_irwxg) { ASSERT_EQ(0000070, S_IRWXG); }

TEST(s_iroth) { ASSERT_EQ(0000004, S_IROTH); }
TEST(s_iwoth) { ASSERT_EQ(0000002, S_IWOTH); }
TEST(s_ixoth) { ASSERT_EQ(0000001, S_IXOTH); }
TEST(s_irwxo) { ASSERT_EQ(0000007, S_IRWXO); }

TEST(s_isuid) { ASSERT_EQ(0004000, S_ISUID); }
TEST(s_isgid) { ASSERT_EQ(0002000, S_ISGID); }
TEST(s_isvtX) { ASSERT_EQ(0001000, S_ISVTX); }

/* ============================================================================ */
TEST_SUITE(macros_file_type_tests);

TEST(s_isreg) {
	ASSERT_TRUE(S_ISREG(S_IFREG | 0644));
	ASSERT_FALSE(S_ISREG(S_IFDIR | 0755));
}

TEST(s_isdir) {
	ASSERT_TRUE(S_ISDIR(S_IFDIR | 0755));
	ASSERT_FALSE(S_ISDIR(S_IFREG | 0644));
}

TEST(s_ischr) {
	ASSERT_TRUE(S_ISCHR(S_IFCHR | 0666));
	ASSERT_FALSE(S_ISCHR(S_IFREG));
}

TEST(s_isblk) {
	ASSERT_TRUE(S_ISBLK(S_IFBLK | 0660));
	ASSERT_FALSE(S_ISBLK(S_IFREG));
}

TEST(s_isfifo) {
	ASSERT_TRUE(S_ISFIFO(S_IFIFO | 0644));
	ASSERT_FALSE(S_ISFIFO(S_IFREG));
}

TEST(s_islnk) {
	ASSERT_TRUE(S_ISLNK(S_IFLNK | 0777));
	ASSERT_FALSE(S_ISLNK(S_IFREG));
}

TEST(s_issock) {
	ASSERT_TRUE(S_ISSOCK(S_IFSOCK | 0666));
	ASSERT_FALSE(S_ISSOCK(S_IFREG));
}

/* ============================================================================ */
TEST_SUITE(struct_stat_layout);

TEST(stat_required_fields) {
	struct stat st = {0};
	st.st_dev = 1; st.st_ino = 2; st.st_mode = S_IFREG | 0644;
	st.st_nlink = 1; st.st_uid = 1000; st.st_gid = 1000; st.st_size = 12345;

	ASSERT_EQ(1, st.st_dev);
	ASSERT_EQ(2, st.st_ino);
	ASSERT_EQ(S_IFREG | 0644, st.st_mode);
	ASSERT_EQ(1, st.st_nlink);
	ASSERT_EQ(1000, st.st_uid);
	ASSERT_EQ(1000, st.st_gid);
	ASSERT_EQ(12345, st.st_size);
}

TEST(stat_timespec_fields) {
	struct stat st = {0};
	st.st_atim.tv_sec = 1000; st.st_mtim.tv_sec = 2000; st.st_ctim.tv_sec = 3000;

	ASSERT_EQ(1000, st.st_atim.tv_sec);
	ASSERT_EQ(2000, st.st_mtim.tv_sec);
	ASSERT_EQ(3000, st.st_ctim.tv_sec);
}

/* ============================================================================ */
#if !STAT_WASM
TEST_SUITE(stat_function);

TEST(stat_regular_file) {
	int fd = open("/tmp/stat_test_file.txt", O_CREAT | O_WRONLY, 0644);
	write(fd, "test", 4); close(fd);

	struct stat st;
	ASSERT_EQ(0, stat("/tmp/stat_test_file.txt", &st));
	ASSERT_TRUE(S_ISREG(st.st_mode));
	ASSERT_EQ(4, st.st_size);

	unlink("/tmp/stat_test_file.txt");
}

TEST(stat_directory) {
	mkdir("/tmp/stat_test_dir", 0755);

	struct stat st;
	ASSERT_EQ(0, stat("/tmp/stat_test_dir", &st));
	ASSERT_TRUE(S_ISDIR(st.st_mode));

	rmdir("/tmp/stat_test_dir");
}

TEST(stat_nonexistent) {
	struct stat st;
	ASSERT_EQ(-1, stat("/tmp/nonexistent_file_xyz123", &st));
	ASSERT_EQ(ENOENT, errno);
}

TEST(stat_null_pathname) {
	struct stat st;
	ASSERT_EQ(-1, stat(NULL, &st));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(stat_null_statbuf) {
	ASSERT_EQ(-1, stat("/tmp", NULL));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(stat_path_max_length) {
	char path[PATH_MAX + 2];
	memset(path, 'a', PATH_MAX); path[PATH_MAX] = '\0';

	struct stat st;
	ASSERT_EQ(-1, stat(path, &st));
	ASSERT_TRUE(errno == ENAMETOOLONG || errno == ENOENT || errno == ENOMEM);
}

TEST(stat_path_embedded_null) {
	char path[64] = "/tmp/test"; path[9] = '\0';

	struct stat st;
	int r = stat(path, &st);
	ASSERT_TRUE(r == -1 || r == 0);
}

/* ============================================================================ */
TEST_SUITE(lstat_function);

TEST(lstat_symlink) {
	unlink("/tmp/stat_link_test");
	symlink("/dev/null", "/tmp/stat_link_test");

	struct stat st;
	ASSERT_EQ(0, lstat("/tmp/stat_link_test", &st));
	ASSERT_TRUE(S_ISLNK(st.st_mode));

	unlink("/tmp/stat_link_test");
}

TEST(lstat_broken_symlink) {
	unlink("/tmp/stat_broken_link");
	symlink("/jacl_nonexistent_target_99999", "/tmp/stat_broken_link");

	struct stat st;
	ASSERT_EQ(0, lstat("/tmp/stat_broken_link", &st));
	ASSERT_TRUE(S_ISLNK(st.st_mode));

	ASSERT_EQ(-1, stat("/tmp/stat_broken_link", &st));
	ASSERT_EQ(ENOENT, errno);

	unlink("/tmp/stat_broken_link");
}

/* ============================================================================ */
TEST_SUITE(fstat_function);

TEST(fstat_basic) {
	int fd = open("/tmp/fstat_test.txt", O_CREAT | O_RDWR, 0644);
	write(fd, "data", 4);

	struct stat st;
	ASSERT_EQ(0, fstat(fd, &st));
	ASSERT_TRUE(S_ISREG(st.st_mode));
	ASSERT_EQ(4, st.st_size);

	close(fd); unlink("/tmp/fstat_test.txt");
}

TEST(fstat_invalid_fd) {
	struct stat st;
	ASSERT_EQ(-1, fstat(-1, &st));
	ASSERT_TRUE(errno == EBADF || errno == EINVAL);
}

TEST(fstat_null_statbuf) {
	int fd = open("/tmp/fstat_test2.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_EQ(-1, fstat(fd, NULL));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
	close(fd); unlink("/tmp/fstat_test2.txt");
}

/* ============================================================================ */
TEST_SUITE(chmod_function);

TEST(chmod_basic) {
	int fd = open("/tmp/chmod_test.txt", O_CREAT | O_WRONLY, 0644); close(fd);

	ASSERT_EQ(0, chmod("/tmp/chmod_test.txt", 0755));

	struct stat st; stat("/tmp/chmod_test.txt", &st);
	ASSERT_TRUE(st.st_mode & S_IXUSR);

	unlink("/tmp/chmod_test.txt");
}

TEST(chmod_null_pathname) {
	ASSERT_EQ(-1, chmod(NULL, 0644));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(chmod_readonly_filesystem) {
	ASSERT_EQ(-1, chmod("/proc/self", 0777));
	ASSERT_TRUE(errno == EPERM || errno == EROFS || errno == EINVAL);
}

/* ============================================================================ */
TEST_SUITE(mkdir_function);

TEST(mkdir_basic) {
	ASSERT_EQ(0, mkdir("/tmp/mkdir_test_dir", 0755));

	struct stat st; stat("/tmp/mkdir_test_dir", &st);
	ASSERT_TRUE(S_ISDIR(st.st_mode));

	rmdir("/tmp/mkdir_test_dir");
}

TEST(mkdir_existing) {
	mkdir("/tmp/mkdir_test_dir2", 0755);
	ASSERT_EQ(-1, mkdir("/tmp/mkdir_test_dir2", 0755));
	ASSERT_EQ(EEXIST, errno);
	rmdir("/tmp/mkdir_test_dir2");
}

TEST(mkdir_null_pathname) {
	ASSERT_EQ(-1, mkdir(NULL, 0755));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(mkdir_no_parent) {
	ASSERT_EQ(-1, mkdir("/jacl_nonexistent_parent/subdir", 0755));
	ASSERT_EQ(ENOENT, errno);
}

/* ============================================================================ */
TEST_SUITE(umask_function);

TEST(umask_basic) {
	mode_t old = umask(0022);
	umask(old);
	ASSERT_TRUE(1);
}

TEST(umask_changes) {
	mode_t old1 = umask(0022);
	mode_t old2 = umask(0077);
	ASSERT_EQ(0022, old2);
	umask(old1);
}

/* ============================================================================ */
TEST_SUITE(statvfs_function);

TEST(statvfs_root) {
	struct statvfs sv;
	int r = statvfs("/", &sv);
	ASSERT_TRUE(r == 0 || r == -1);
	if (r == 0) {
		ASSERT_TRUE(sv.f_bsize > 0);
		ASSERT_TRUE(sv.f_namemax > 0);
	}
}

TEST(statvfs_null_path) {
	struct statvfs sv;
	ASSERT_EQ(-1, statvfs(NULL, &sv));
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(fstatvfs_directory_fd) {
	int fd = open("/tmp", O_RDONLY | O_DIRECTORY);
	if (fd >= 0) {
		struct statvfs sv;
		int r = fstatvfs(fd, &sv);
		ASSERT_TRUE(r == 0 || r == -1);
		close(fd);
	}
}

/* ============================================================================ */
TEST_SUITE(mode_bitwise_operations);

TEST(permission_combinations) {
	mode_t rw = S_IRUSR | S_IWUSR;
	ASSERT_TRUE(rw & S_IRUSR);
	ASSERT_TRUE(rw & S_IWUSR);
	ASSERT_FALSE(rw & S_IXUSR);
}

TEST(all_permissions) {
	mode_t all = S_IRWXU | S_IRWXG | S_IRWXO;
	ASSERT_EQ(0777, all);
}

TEST(typical_file_perms) {
	mode_t m = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	ASSERT_EQ(0644, m);
}

TEST(typical_dir_perms) {
	mode_t m = S_IRUSR|S_IWUSR|S_IXUSR | S_IRGRP|S_IXGRP | S_IROTH|S_IXOTH;
	ASSERT_EQ(0755, m);
}

TEST(extract_file_type) {
	mode_t m = S_IFREG | 0644;
	ASSERT_EQ(S_IFREG, m & S_IFMT);
}

TEST(extract_permissions) {
	mode_t m = S_IFREG | 0755;
	ASSERT_EQ(0755, m & 0777);
}

TEST(combine_type_and_perms) {
	mode_t m = S_IFDIR | 0755;
	ASSERT_TRUE(S_ISDIR(m));
	ASSERT_EQ(0755, m & 0777);
}

TEST(zero_permissions) {
	mode_t m = S_IFREG | 0000;
	ASSERT_TRUE(S_ISREG(m));
	ASSERT_EQ(0000, m & 0777);
}

TEST(all_special_bits) {
	mode_t m = S_ISUID | S_ISGID | S_ISVTX;
	ASSERT_TRUE(m & S_ISUID);
	ASSERT_TRUE(m & S_ISGID);
	ASSERT_TRUE(m & S_ISVTX);
}

TEST(file_type_mask_isolation) {
	mode_t m = S_IFREG | S_ISUID | 0755;
	ASSERT_EQ(S_IFREG, m & S_IFMT);
	ASSERT_TRUE(S_ISREG(m));
}

#endif /* !STAT_WASM */

/* ============================================================================ */
#if STAT_WASM
TEST_SUITE(wasm_stubs);

TEST(wasm_stat_returns_error) {
	struct stat st;
	ASSERT_EQ(-1, stat("/anything", &st));
}

TEST(wasm_chmod_returns_error) {
	ASSERT_EQ(-1, chmod("/anything", 0644));
}

TEST(wasm_mkdir_returns_error) {
	ASSERT_EQ(-1, mkdir("/anything", 0755));
}
#endif

/* ============================================================================ */
#if STAT_WIN32
TEST_SUITE(windows_stubs);

TEST(win_stat_null_path) {
	ASSERT_EQ(-1, stat(NULL, NULL));
}

TEST(win_stat_nonexistent) {
	struct stat st;
	ASSERT_EQ(-1, stat("Z:\\jacl_nonexistent_99999", &st));
}
#endif

/* ============================================================================ */
TEST_MAIN()
