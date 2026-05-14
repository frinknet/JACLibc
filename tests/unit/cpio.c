/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <cpio.h>
#include <sys/stat.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(cpio.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_magic_and_types) {
	ASSERT_STR_EQ(MAGIC, "070707");
	ASSERT_EQ(6, strlen(MAGIC));

	ASSERT_EQ(S_IFREG, C_ISREG);
	ASSERT_EQ(S_IFDIR, C_ISDIR);
	ASSERT_EQ(S_IFCHR, C_ISCHR);
	ASSERT_EQ(S_IFBLK, C_ISBLK);
	ASSERT_EQ(S_IFIFO, C_ISFIFO);
	ASSERT_EQ(S_IFLNK, C_ISLNK);
	ASSERT_EQ(S_IFSOCK, C_ISSOCK);
	ASSERT_EQ(0110000, C_ISCTG);

	ASSERT_NE(C_ISREG, C_ISDIR);
	ASSERT_NE(C_ISCTG, S_IFMT & ~C_ISCTG);
}

TEST(constants_permissions_equivalence) {
	ASSERT_EQ(S_IRUSR, C_IRUSR);
	ASSERT_EQ(S_IWUSR, C_IWUSR);
	ASSERT_EQ(S_IXUSR, C_IXUSR);
	ASSERT_EQ(S_IRGRP, C_IRGRP);
	ASSERT_EQ(S_IWGRP, C_IWGRP);
	ASSERT_EQ(S_IXGRP, C_IXGRP);
	ASSERT_EQ(S_IROTH, C_IROTH);
	ASSERT_EQ(S_IWOTH, C_IWOTH);
	ASSERT_EQ(S_IXOTH, C_IXOTH);
	ASSERT_EQ(S_ISUID, C_ISUID);
	ASSERT_EQ(S_ISGID, C_ISGID);
	ASSERT_EQ(S_ISVTX, C_ISVTX);
}

TEST(constants_mode_operations) {
	mode_t rw = C_IRUSR | C_IWUSR;
	ASSERT_TRUE(rw & C_IRUSR);
	ASSERT_TRUE(rw & C_IWUSR);
	ASSERT_FALSE(rw & C_IXUSR);

	mode_t all = (C_IRUSR|C_IWUSR|C_IXUSR) |
	             (C_IRGRP|C_IWGRP|C_IXGRP) |
	             (C_IROTH|C_IWOTH|C_IXOTH);
	ASSERT_EQ(0777, all);

	mode_t m = C_ISDIR | 0755;
	ASSERT_TRUE(S_ISDIR(m));
	ASSERT_EQ(0755, m & 0777);

	mode_t special = C_ISUID | C_ISGID | C_ISVTX;
	ASSERT_TRUE(special & C_ISUID);
	ASSERT_TRUE(special & C_ISGID);
	ASSERT_TRUE(special & C_ISVTX);
}

/* ============================================================================ */
TEST_MAIN()
