/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <tar.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(tar.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_magic_and_version) {
	ASSERT_STR_EQ(TMAGIC, "ustar");
	ASSERT_EQ(6, TMAGLEN);
	ASSERT_STR_EQ(TVERSION, "00");
	ASSERT_EQ(2, TVERSLEN);
	
	/* TMAGIC includes null in TMAGLEN (6 bytes for "ustar\0") */
	ASSERT_EQ(strlen(TMAGIC), TMAGLEN - 1);
	
	/* TVERSION does NOT include null in TVERSLEN (2 bytes for "00") */
	ASSERT_EQ(strlen(TVERSION), TVERSLEN);
}

TEST(constants_typeflags) {
	ASSERT_EQ(REGTYPE, '0');
	ASSERT_EQ(AREGTYPE, '\0');
	ASSERT_EQ(LNKTYPE, '1');
	ASSERT_EQ(SYMTYPE, '2');
	ASSERT_EQ(CHRTYPE, '3');
	ASSERT_EQ(BLKTYPE, '4');
	ASSERT_EQ(DIRTYPE, '5');
	ASSERT_EQ(FIFOTYPE, '6');
	ASSERT_EQ(CONTTYPE, '7');

	/* Ensure uniqueness */
	ASSERT_NE(REGTYPE, LNKTYPE);
	ASSERT_NE(SYMTYPE, DIRTYPE);
	ASSERT_NE(AREGTYPE, REGTYPE);
}

TEST(constants_mode_bits_owner) {
	ASSERT_EQ(TSUID, 04000);
	ASSERT_EQ(TUREAD, 00400);
	ASSERT_EQ(TUWRITE, 00200);
	ASSERT_EQ(TUEXEC, 00100);

	mode_t owner_rw = TUREAD | TUWRITE;
	ASSERT_TRUE(owner_rw & TUREAD);
	ASSERT_TRUE(owner_rw & TUWRITE);
	ASSERT_FALSE(owner_rw & TUEXEC);
}

TEST(constants_mode_bits_group) {
	ASSERT_EQ(TSGID, 02000);
	ASSERT_EQ(TGREAD, 00040);
	ASSERT_EQ(TGWRITE, 00020);
	ASSERT_EQ(TGEXEC, 00010);

	mode_t group_exec = TGEXEC;
	ASSERT_TRUE(group_exec & TGEXEC);
	ASSERT_FALSE(group_exec & TGREAD);
}

TEST(constants_mode_bits_other) {
	ASSERT_EQ(TSVTX, 01000);
	ASSERT_EQ(TOREAD, 00004);
	ASSERT_EQ(TOWRITE, 00002);
	ASSERT_EQ(TOEXEC, 00001);

	mode_t other_all = TOREAD | TOWRITE | TOEXEC;
	ASSERT_EQ(other_all, 00007);
}

TEST(constants_mode_combinations) {
	/* Standard 755 directory */
	mode_t dir_mode = TUREAD | TUWRITE | TUEXEC |
	                  TGREAD | TGEXEC |
	                  TOREAD | TOEXEC;
	ASSERT_EQ(dir_mode, 0755);

	/* Standard 644 file */
	mode_t file_mode = TUREAD | TUWRITE |
	                   TGREAD |
	                   TOREAD;
	ASSERT_EQ(file_mode, 0644);

	/* Setuid binary */
	mode_t suid_mode = TSUID | TUREAD | TUEXEC;
	ASSERT_TRUE(suid_mode & TSUID);
	ASSERT_EQ(suid_mode & 0777, 0500);
}

/* ============================================================================ */
TEST_MAIN()
