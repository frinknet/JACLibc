/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <dirent.h>

TEST_TYPE(unit);
TEST_UNIT(dirent.h);

/* Test directory setup */
static const char *test_dir = "/tmp/dirent_test";

/* ============================================================================
 * SETUP HELPERS
 * ============================================================================ */

static void cleanup_test_directory(void) {
	char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/file1.txt", test_dir);
	unlink(path);
	snprintf(path, sizeof(path), "%s/file2.txt", test_dir);
	unlink(path);
	snprintf(path, sizeof(path), "%s/subdir", test_dir);
	rmdir(path);
	rmdir(test_dir);
}

static void create_test_directory(void) {
	/* Clean up first if exists */
	cleanup_test_directory();

	/* Create test directory */
	int ret = mkdir(test_dir, 0755);
	ASSERT_EQ(0, ret);

	/* Create test files */
	char path[PATH_MAX];

	snprintf(path, sizeof(path), "%s/file1.txt", test_dir);
	int fd = open(path, O_CREAT | O_WRONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);

	snprintf(path, sizeof(path), "%s/file2.txt", test_dir);
	fd = open(path, O_CREAT | O_WRONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);

	snprintf(path, sizeof(path), "%s/subdir", test_dir);
	ret = mkdir(path, 0755);
	ASSERT_EQ(0, ret);
}

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(constants_name_max) {
	ASSERT_TRUE(NAME_MAX > 0);
	ASSERT_EQ(255, NAME_MAX);
}

TEST(constants_path_max) {
	ASSERT_TRUE(PATH_MAX > 0);
	ASSERT_EQ(4096, PATH_MAX);
}

TEST(constants_d_type_values) {
	ASSERT_EQ(0, DT_UNKNOWN);
	ASSERT_EQ(1, DT_FIFO);
	ASSERT_EQ(2, DT_CHR);
	ASSERT_EQ(4, DT_DIR);
	ASSERT_EQ(6, DT_BLK);
	ASSERT_EQ(8, DT_REG);
	ASSERT_EQ(10, DT_LNK);
	ASSERT_EQ(12, DT_SOCK);
	ASSERT_EQ(14, DT_WHT);
}

/* ============================================================================
 * STRUCT: dirent
 * ============================================================================ */
TEST_SUITE(dirent);

TEST(dirent_members) {
	dirent d;
	d.d_ino = 12345;
	d.d_type = DT_REG;
	strcpy(d.d_name, "test.txt");
	ASSERT_EQ(12345, d.d_ino);
	ASSERT_EQ(DT_REG, d.d_type);
	ASSERT_STR_EQ("test.txt", d.d_name);
}

TEST(dirent_name_max_size) {
	dirent d;
	ASSERT_TRUE(sizeof(d.d_name) >= NAME_MAX + 1);
}

/* ============================================================================
 * STRUCT: DIR
 * ============================================================================ */
TEST_SUITE(DIR);

TEST(DIR_opaque) {
	DIR *dirp = NULL;
	ASSERT_NULL(dirp);
}

/* ============================================================================
 * MACRO: IFTODT
 * ============================================================================ */
TEST_SUITE(IFTODT);

TEST(IFTODT_regular) {
	unsigned int mode = S_IFREG;
	unsigned char dtype = IFTODT(mode);
	ASSERT_EQ(DT_REG, dtype);
}

TEST(IFTODT_directory) {
	unsigned int mode = S_IFDIR;
	unsigned char dtype = IFTODT(mode);
	ASSERT_EQ(DT_DIR, dtype);
}

TEST(IFTODT_symlink) {
	unsigned int mode = S_IFLNK;
	unsigned char dtype = IFTODT(mode);
	ASSERT_EQ(DT_LNK, dtype);
}

/* ============================================================================
 * MACRO: DTTOIF
 * ============================================================================ */
TEST_SUITE(DTTOIF);

TEST(DTTOIF_regular) {
	unsigned int mode = DTTOIF(DT_REG);
	ASSERT_EQ(S_IFREG, mode);
}

TEST(DTTOIF_directory) {
	unsigned int mode = DTTOIF(DT_DIR);
	ASSERT_EQ(S_IFDIR, mode);
}

TEST(DTTOIF_symlink) {
	unsigned int mode = DTTOIF(DT_LNK);
	ASSERT_EQ(S_IFLNK, mode);
}

TEST(DTTOIF_IFTODT_roundtrip) {
	unsigned int original = S_IFREG;
	unsigned char dtype = IFTODT(original);
	unsigned int converted = DTTOIF(dtype);
	ASSERT_EQ(original, converted);
}

/* ============================================================================
 * FUNCTION: opendir
 * ============================================================================ */
TEST_SUITE(opendir);

TEST(opendir_valid_directory) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	ASSERT_NOT_NULL(dir);
	closedir(dir);
	cleanup_test_directory();
}

TEST(opendir_null_path) {
	DIR *dir = opendir(NULL);
	ASSERT_NULL(dir);
	ASSERT_EQ(EINVAL, errno);
}

TEST(opendir_nonexistent) {
	DIR *dir = opendir("/tmp/nonexistent_dir_xyz123");
	ASSERT_NULL(dir);
}

/* ============================================================================
 * FUNCTION: closedir
 * ============================================================================ */
TEST_SUITE(closedir);

TEST(closedir_valid) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	int result = closedir(dir);
	ASSERT_EQ(0, result);
	cleanup_test_directory();
}

TEST(closedir_null) {
	int result = closedir(NULL);
	ASSERT_EQ(-1, result);
	ASSERT_EQ(EBADF, errno);
}

TEST(closedir_double) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	closedir(dir);
	int result = closedir(dir);
	ASSERT_EQ(0, result);
	cleanup_test_directory();
}

/* ============================================================================
 * FUNCTION: readdir
 * ============================================================================ */
TEST_SUITE(readdir);

TEST(readdir_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	ASSERT_NOT_NULL(dir);
	int count = 0;
	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		ASSERT_NOT_NULL(entry->d_name);
		ASSERT_TRUE(strlen(entry->d_name) > 0);
		count++;
	}
	ASSERT_TRUE(count >= 3);
	closedir(dir);
	cleanup_test_directory();
}

TEST(readdir_finds_dot_entries) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	int found_dot = 0, found_dotdot = 0;
	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0) found_dot = 1;
		if (strcmp(entry->d_name, "..") == 0) found_dotdot = 1;
	}
	ASSERT_TRUE(found_dot);
	ASSERT_TRUE(found_dotdot);
	closedir(dir);
	cleanup_test_directory();
}

TEST(readdir_null_dir) {
	dirent *entry = readdir(NULL);
	ASSERT_NULL(entry);
	ASSERT_EQ(EBADF, errno);
}

TEST(readdir_end_of_directory) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	dirent *entry;
	while ((entry = readdir(dir)) != NULL);
	entry = readdir(dir);
	ASSERT_NULL(entry);
	closedir(dir);
	cleanup_test_directory();
}

TEST(readdir_d_type_regular) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	ASSERT_NOT_NULL(dir);
	dirent *entry;
	int found_regular = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, "file1.txt") == 0 ||
		    strcmp(entry->d_name, "file2.txt") == 0) {
			ASSERT_EQ(DT_REG, entry->d_type);
			found_regular = 1;
		}
	}
	ASSERT_TRUE(found_regular);
	closedir(dir);
	cleanup_test_directory();
}

TEST(readdir_d_type_directory) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	ASSERT_NOT_NULL(dir);
	dirent *entry;
	int found_dir = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, "subdir") == 0) {
			ASSERT_EQ(DT_DIR, entry->d_type);
			found_dir = 1;
		}
	}
	ASSERT_TRUE(found_dir);
	closedir(dir);
	cleanup_test_directory();
}

/* ============================================================================
 * FUNCTION: readdir_r
 * ============================================================================ */
TEST_SUITE(readdir_r);

TEST(readdir_r_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	ASSERT_NOT_NULL(dir);
	dirent entry_buf, *result;
	int count = 0;
	while (readdir_r(dir, &entry_buf, &result) == 0 && result != NULL) {
		ASSERT_NOT_NULL(result->d_name);
		count++;
	}
	ASSERT_TRUE(count >= 3);
	closedir(dir);
	cleanup_test_directory();
}

TEST(readdir_r_null_params) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	int ret = readdir_r(NULL, NULL, NULL);
	ASSERT_EQ(EINVAL, ret);
	closedir(dir);
	cleanup_test_directory();
}

/* ============================================================================
 * FUNCTION: rewinddir
 * ============================================================================ */
TEST_SUITE(rewinddir);

TEST(rewinddir_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	readdir(dir);
	readdir(dir);
	rewinddir(dir);
	dirent *entry = readdir(dir);
	ASSERT_NOT_NULL(entry);
	closedir(dir);
	cleanup_test_directory();
}

TEST(rewinddir_null) {
	rewinddir(NULL);
	ASSERT_TRUE(1);
}

/* ============================================================================
 * FUNCTION: telldir
 * ============================================================================ */
TEST_SUITE(telldir);

TEST(telldir_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	long pos = telldir(dir);
	ASSERT_TRUE(pos >= 0);
	closedir(dir);
	cleanup_test_directory();
}

TEST(telldir_increments) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	long pos1 = telldir(dir);
	readdir(dir);
	long pos2 = telldir(dir);
	ASSERT_TRUE(pos2 > pos1);
	closedir(dir);
	cleanup_test_directory();
}

TEST(telldir_null) {
	long pos = telldir(NULL);
	ASSERT_EQ(-1, pos);
	ASSERT_EQ(EBADF, errno);
}

/* ============================================================================
 * FUNCTION: seekdir
 * ============================================================================ */
TEST_SUITE(seekdir);

TEST(seekdir_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	long pos = telldir(dir);
	readdir(dir);
	readdir(dir);
	seekdir(dir, pos);
	long new_pos = telldir(dir);
	ASSERT_EQ(pos, new_pos);
	closedir(dir);
	cleanup_test_directory();
}

TEST(seekdir_null) {
	seekdir(NULL, 0);
	ASSERT_TRUE(1);
}

TEST(seekdir_negative) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	seekdir(dir, -1);
	long pos = telldir(dir);
	ASSERT_TRUE(pos >= 0);
	closedir(dir);
	cleanup_test_directory();
}

/* ============================================================================
 * FUNCTION: dirfd
 * ============================================================================ */
#if DIRENT_POSIX
TEST_SUITE(dirfd);

TEST(dirfd_basic) {
	create_test_directory();
	DIR *dir = opendir(test_dir);
	int fd = dirfd(dir);
	ASSERT_TRUE(fd >= 0);
	closedir(dir);
	cleanup_test_directory();
}

TEST(dirfd_null) {
	int fd = dirfd(NULL);
	ASSERT_EQ(-1, fd);
	ASSERT_EQ(EBADF, errno);
}
#endif

/* ============================================================================
 * FUNCTION: fdopendir
 * ============================================================================ */
#if DIRENT_POSIX
TEST_SUITE(fdopendir);

TEST(fdopendir_basic) {
	create_test_directory();
	int fd = open(test_dir, O_RDONLY);
	ASSERT_TRUE(fd >= 0);
	DIR *dir = fdopendir(fd);
	ASSERT_NOT_NULL(dir);
	closedir(dir);
	cleanup_test_directory();
}

TEST(fdopendir_invalid_fd) {
	DIR *dir = fdopendir(-1);
	ASSERT_NULL(dir);
	ASSERT_EQ(EBADF, errno);
}
#endif

/* ============================================================================
 * FUNCTION: alphasort
 * ============================================================================ */
TEST_SUITE(alphasort);

TEST(alphasort_basic) {
	dirent a, b;
	strcpy(a.d_name, "apple");
	strcpy(b.d_name, "banana");
	dirent *pa = &a, *pb = &b;
	int result = alphasort(&pa, &pb);
	ASSERT_TRUE(result < 0);
}

TEST(alphasort_equal) {
	dirent a, b;
	strcpy(a.d_name, "test");
	strcpy(b.d_name, "test");
	dirent *pa = &a, *pb = &b;
	int result = alphasort(&pa, &pb);
	ASSERT_EQ(0, result);
}

TEST(alphasort_reverse) {
	dirent a, b;
	strcpy(a.d_name, "zebra");
	strcpy(b.d_name, "apple");
	dirent *pa = &a, *pb = &b;
	int result = alphasort(&pa, &pb);
	ASSERT_TRUE(result > 0);
}

/* ============================================================================
 * FUNCTION: versionsort
 * ============================================================================ */
TEST_SUITE(versionsort);

TEST(versionsort_basic) {
	dirent a, b;
	strcpy(a.d_name, "apple");
	strcpy(b.d_name, "banana");
	dirent *pa = &a, *pb = &b;
	int result = versionsort(&pa, &pb);
	ASSERT_TRUE(result < 0);
}

TEST(versionsort_equal) {
	dirent a, b;
	strcpy(a.d_name, "test");
	strcpy(b.d_name, "test");
	dirent *pa = &a, *pb = &b;
	int result = versionsort(&pa, &pb);
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * FUNCTION: scandir
 * ============================================================================ */
TEST_SUITE(scandir);

TEST(scandir_basic) {
	create_test_directory();
	dirent **namelist;
	int n = scandir(test_dir, &namelist, NULL, alphasort);
	ASSERT_TRUE(n >= 3);
	int sorted = 1;
	for (int i = 1; i < n; i++) {
		if (strcmp(namelist[i-1]->d_name, namelist[i]->d_name) > 0) {
			sorted = 0;
			break;
		}
	}
	ASSERT_TRUE(sorted);
	for (int i = 0; i < n; i++) free(namelist[i]);
	free(namelist);
	cleanup_test_directory();
}

TEST(scandir_null_path) {
	dirent **namelist;
	int n = scandir(NULL, &namelist, NULL, NULL);
	ASSERT_EQ(-1, n);
	ASSERT_EQ(EINVAL, errno);
}

TEST(scandir_nonexistent) {
	dirent **namelist;
	int n = scandir("/tmp/nonexistent_xyz", &namelist, NULL, NULL);
	ASSERT_EQ(-1, n);
}

static int filter_txt_files(const dirent *entry) {
	if (!entry || !entry->d_name[0]) return 0;
	size_t len = strlen(entry->d_name);
	if (len < 5) return 0;
	return (entry->d_name[len-4] == '.' &&
	        entry->d_name[len-3] == 't' &&
	        entry->d_name[len-2] == 'x' &&
	        entry->d_name[len-1] == 't');
}

TEST(scandir_with_filter) {
	create_test_directory();
	dirent **namelist;
	int n = scandir(test_dir, &namelist, filter_txt_files, alphasort);
	ASSERT_EQ(2, n);
	for (int i = 0; i < n; i++) free(namelist[i]);
	free(namelist);
	cleanup_test_directory();
}

TEST(scandir_null_namelist) {
	int n = scandir(test_dir, NULL, NULL, NULL);
	ASSERT_EQ(-1, n);
	ASSERT_EQ(EINVAL, errno);
}

TEST_MAIN()
