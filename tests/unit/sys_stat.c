/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

TEST_TYPE(unit);
TEST_UNIT(sys/stat.h);

/* ============================================================================
 * CONSTANTS - FILE TYPE MASKS
 * ============================================================================ */
TEST_SUITE(file_type_masks);

TEST(file_type_constants) {
	ASSERT_EQ(0170000, S_IFMT);
	ASSERT_EQ(0140000, S_IFSOCK);
	ASSERT_EQ(0120000, S_IFLNK);
	ASSERT_EQ(0100000, S_IFREG);
	ASSERT_EQ(0060000, S_IFBLK);
	ASSERT_EQ(0040000, S_IFDIR);
	ASSERT_EQ(0020000, S_IFCHR);
	ASSERT_EQ(0010000, S_IFIFO);
}

TEST(file_types_unique) {
	ASSERT_NE(S_IFREG, S_IFDIR);
	ASSERT_NE(S_IFDIR, S_IFCHR);
	ASSERT_NE(S_IFREG, S_IFLNK);
}

/* ============================================================================
 * CONSTANTS - PERMISSIONS
 * ============================================================================ */
TEST_SUITE(permission_constants);

TEST(owner_permissions) {
	ASSERT_EQ(0000400, S_IRUSR);
	ASSERT_EQ(0000200, S_IWUSR);
	ASSERT_EQ(0000100, S_IXUSR);
	ASSERT_EQ(0000700, S_IRWXU);
}

TEST(group_permissions) {
	ASSERT_EQ(0000040, S_IRGRP);
	ASSERT_EQ(0000020, S_IWGRP);
	ASSERT_EQ(0000010, S_IXGRP);
	ASSERT_EQ(0000070, S_IRWXG);
}

TEST(other_permissions) {
	ASSERT_EQ(0000004, S_IROTH);
	ASSERT_EQ(0000002, S_IWOTH);
	ASSERT_EQ(0000001, S_IXOTH);
	ASSERT_EQ(0000007, S_IRWXO);
}

TEST(special_mode_bits) {
	ASSERT_EQ(0004000, S_ISUID);
	ASSERT_EQ(0002000, S_ISGID);
	ASSERT_EQ(0001000, S_ISVTX);
}

/* ============================================================================
 * MACROS - FILE TYPE TESTS
 * ============================================================================ */
TEST_SUITE(file_type_macros);

TEST(s_isreg_macro) {
	ASSERT_TRUE(S_ISREG(S_IFREG | 0644));
	ASSERT_FALSE(S_ISREG(S_IFDIR | 0755));
	ASSERT_FALSE(S_ISREG(S_IFCHR));
}

TEST(s_isdir_macro) {
	ASSERT_TRUE(S_ISDIR(S_IFDIR | 0755));
	ASSERT_FALSE(S_ISDIR(S_IFREG | 0644));
	ASSERT_FALSE(S_ISDIR(S_IFLNK));
}

TEST(s_ischr_macro) {
	ASSERT_TRUE(S_ISCHR(S_IFCHR | 0666));
	ASSERT_FALSE(S_ISCHR(S_IFREG));
	ASSERT_FALSE(S_ISCHR(S_IFBLK));
}

TEST(s_isblk_macro) {
	ASSERT_TRUE(S_ISBLK(S_IFBLK | 0660));
	ASSERT_FALSE(S_ISBLK(S_IFREG));
	ASSERT_FALSE(S_ISBLK(S_IFCHR));
}

TEST(s_isfifo_macro) {
	ASSERT_TRUE(S_ISFIFO(S_IFIFO | 0644));
	ASSERT_FALSE(S_ISFIFO(S_IFREG));
	ASSERT_FALSE(S_ISFIFO(S_IFDIR));
}

TEST(s_islnk_macro) {
	ASSERT_TRUE(S_ISLNK(S_IFLNK | 0777));
	ASSERT_FALSE(S_ISLNK(S_IFREG));
	ASSERT_FALSE(S_ISLNK(S_IFDIR));
}

TEST(s_issock_macro) {
	ASSERT_TRUE(S_ISSOCK(S_IFSOCK | 0666));
	ASSERT_FALSE(S_ISSOCK(S_IFREG));
	ASSERT_FALSE(S_ISSOCK(S_IFDIR));
}

/* ============================================================================
 * STAT STRUCTURE
 * ============================================================================ */
TEST_SUITE(stat_structure);

TEST(stat_has_required_fields) {
	struct stat st;
	
	st.st_dev = 1;
	st.st_ino = 2;
	st.st_mode = S_IFREG | 0644;
	st.st_nlink = 1;
	st.st_uid = 1000;
	st.st_gid = 1000;
	st.st_size = 12345;
	
	ASSERT_EQ(1, st.st_dev);
	ASSERT_EQ(2, st.st_ino);
	ASSERT_EQ(S_IFREG | 0644, st.st_mode);
	ASSERT_EQ(1, st.st_nlink);
	ASSERT_EQ(1000, st.st_uid);
	ASSERT_EQ(1000, st.st_gid);
	ASSERT_EQ(12345, st.st_size);
}

TEST(stat_has_timespec_fields) {
	struct stat st;
	
	st.st_atim.tv_sec = 1000;
	st.st_mtim.tv_sec = 2000;
	st.st_ctim.tv_sec = 3000;
	
	ASSERT_EQ(1000, st.st_atim.tv_sec);
	ASSERT_EQ(2000, st.st_mtim.tv_sec);
	ASSERT_EQ(3000, st.st_ctim.tv_sec);
}

/* ============================================================================
 * STAT - BASIC OPERATIONS
 * ============================================================================ */
#if !STAT_WASM
TEST_SUITE(stat_basic);

TEST(stat_regular_file) {
	// Create test file
	int fd = open("/tmp/stat_test_file.txt", O_CREAT | O_WRONLY, 0644);
	write(fd, "test", 4);
	close(fd);
	
	struct stat st;
	int result = stat("/tmp/stat_test_file.txt", &st);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(S_ISREG(st.st_mode));
	ASSERT_EQ(4, st.st_size);
	
	unlink("/tmp/stat_test_file.txt");
}

TEST(stat_directory) {
	mkdir("/tmp/stat_test_dir", 0755);
	
	struct stat st;
	int result = stat("/tmp/stat_test_dir", &st);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(S_ISDIR(st.st_mode));
	
	rmdir("/tmp/stat_test_dir");
}

TEST(stat_nonexistent) {
	struct stat st;
	int result = stat("/tmp/nonexistent_file_xyz123", &st);
	
	ASSERT_EQ(-1, result);
}

TEST(stat_null_pathname) {
	struct stat st;
	int result = stat(NULL, &st);
	
	ASSERT_EQ(-1, result);
}

TEST(stat_null_statbuf) {
	int result = stat("/tmp", NULL);
	
	ASSERT_EQ(-1, result);
}
#endif

/* ============================================================================
 * FSTAT
 * ============================================================================ */
#if !STAT_WASM
TEST_SUITE(fstat_test);

TEST(fstat_basic) {
	int fd = open("/tmp/fstat_test.txt", O_CREAT | O_RDWR, 0644);
	write(fd, "data", 4);
	
	struct stat st;
	int result = fstat(fd, &st);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(S_ISREG(st.st_mode));
	ASSERT_EQ(4, st.st_size);
	
	close(fd);
	unlink("/tmp/fstat_test.txt");
}

TEST(fstat_invalid_fd) {
	struct stat st;
	int result = fstat(-1, &st);
	
	ASSERT_EQ(-1, result);
}

TEST(fstat_null_statbuf) {
	int fd = open("/tmp/fstat_test2.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fstat(fd, NULL);
	
	ASSERT_EQ(-1, result);
	
	close(fd);
	unlink("/tmp/fstat_test2.txt");
}
#endif

/* ============================================================================
 * CHMOD
 * ============================================================================ */
#if !STAT_WASM
TEST_SUITE(chmod_test);

TEST(chmod_basic) {
	int fd = open("/tmp/chmod_test.txt", O_CREAT | O_WRONLY, 0644);
	close(fd);
	
	int result = chmod("/tmp/chmod_test.txt", 0755);
	
	ASSERT_EQ(0, result);
	
	struct stat st;
	stat("/tmp/chmod_test.txt", &st);
	ASSERT_TRUE(st.st_mode & S_IXUSR);
	
	unlink("/tmp/chmod_test.txt");
}

TEST(chmod_null_pathname) {
	int result = chmod(NULL, 0644);
	
	ASSERT_EQ(-1, result);
}
#endif

/* ============================================================================
 * MKDIR
 * ============================================================================ */
#if !STAT_WASM
TEST_SUITE(mkdir_test);

TEST(mkdir_basic) {
	int result = mkdir("/tmp/mkdir_test_dir", 0755);
	
	ASSERT_EQ(0, result);
	
	struct stat st;
	stat("/tmp/mkdir_test_dir", &st);
	ASSERT_TRUE(S_ISDIR(st.st_mode));
	
	rmdir("/tmp/mkdir_test_dir");
}

TEST(mkdir_existing) {
	mkdir("/tmp/mkdir_test_dir2", 0755);
	
	int result = mkdir("/tmp/mkdir_test_dir2", 0755);
	
	ASSERT_EQ(-1, result);
	
	rmdir("/tmp/mkdir_test_dir2");
}

TEST(mkdir_null_pathname) {
	int result = mkdir(NULL, 0755);
	
	ASSERT_EQ(-1, result);
}
#endif

/* ============================================================================
 * UMASK
 * ============================================================================ */
TEST_SUITE(umask_test);

TEST(umask_basic) {
	mode_t old_mask = umask(0022);
	
	// Set it back
	umask(old_mask);
	
	ASSERT_TRUE(1); // Just verify it doesn't crash
}

TEST(umask_changes) {
	mode_t old1 = umask(0022);
	mode_t old2 = umask(0077);
	
	ASSERT_EQ(0022, old2);
	
	umask(old1);
}

/* ============================================================================
 * PERMISSION COMBINATIONS
 * ============================================================================ */
TEST_SUITE(permission_combinations);

TEST(read_write_permission) {
	mode_t mode = S_IRUSR | S_IWUSR;
	
	ASSERT_TRUE(mode & S_IRUSR);
	ASSERT_TRUE(mode & S_IWUSR);
	ASSERT_FALSE(mode & S_IXUSR);
}

TEST(all_permissions) {
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	
	ASSERT_EQ(0777, mode);
}

TEST(typical_file_permissions) {
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	
	ASSERT_EQ(0644, mode);
}

TEST(typical_directory_permissions) {
	mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	
	ASSERT_EQ(0755, mode);
}

/* ============================================================================
 * MODE EXTRACTION
 * ============================================================================ */
TEST_SUITE(mode_extraction);

TEST(extract_file_type) {
	mode_t mode = S_IFREG | 0644;
	mode_t type = mode & S_IFMT;
	
	ASSERT_EQ(S_IFREG, type);
}

TEST(extract_permissions) {
	mode_t mode = S_IFREG | 0755;
	mode_t perms = mode & 0777;
	
	ASSERT_EQ(0755, perms);
}

TEST(combine_type_and_perms) {
	mode_t type = S_IFDIR;
	mode_t perms = 0755;
	mode_t mode = type | perms;
	
	ASSERT_TRUE(S_ISDIR(mode));
	ASSERT_EQ(0755, mode & 0777);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_permissions) {
	mode_t mode = S_IFREG | 0000;
	
	ASSERT_TRUE(S_ISREG(mode));
	ASSERT_EQ(0000, mode & 0777);
}

TEST(all_special_bits) {
	mode_t mode = S_ISUID | S_ISGID | S_ISVTX;
	
	ASSERT_TRUE(mode & S_ISUID);
	ASSERT_TRUE(mode & S_ISGID);
	ASSERT_TRUE(mode & S_ISVTX);
}

TEST(file_type_mask_isolation) {
	mode_t mode = S_IFREG | S_ISUID | 0755;
	mode_t type = mode & S_IFMT;
	
	ASSERT_EQ(S_IFREG, type);
	ASSERT_TRUE(S_ISREG(mode));
}

TEST_MAIN()
