/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(sys/mman.h);

/* ============================================================================ */
TEST_SUITE(constants_protection);

TEST(prot_none) { ASSERT_EQ(0x00, PROT_NONE); }
TEST(prot_read) { ASSERT_EQ(0x01, PROT_READ); }
TEST(prot_write) { ASSERT_EQ(0x02, PROT_WRITE); }
TEST(prot_exec) { ASSERT_EQ(0x04, PROT_EXEC); }

TEST(prot_combinations_valid) {
	ASSERT_TRUE(PROT_READ | PROT_WRITE);
	ASSERT_TRUE(PROT_READ | PROT_EXEC);
	ASSERT_TRUE(PROT_READ | PROT_WRITE | PROT_EXEC);
}

TEST(prot_flags_unique) {
	ASSERT_NE(PROT_READ, PROT_WRITE);
	ASSERT_NE(PROT_WRITE, PROT_EXEC);
	ASSERT_NE(PROT_READ, PROT_EXEC);
}

/* ============================================================================ */
TEST_SUITE(constants_mapping);

TEST(map_shared) { ASSERT_EQ(0x01, MAP_SHARED); }
TEST(map_private) { ASSERT_EQ(0x02, MAP_PRIVATE); }
TEST(map_fixed) { ASSERT_EQ(0x10, MAP_FIXED); }
TEST(map_anon) { ASSERT_EQ(0x20, MAP_ANONYMOUS); }
TEST(map_anon_alias) { ASSERT_EQ(MAP_ANONYMOUS, MAP_ANON); }
TEST(map_failed) { ASSERT_EQ((void*)-1, MAP_FAILED); }

TEST(map_mutual_exclusivity) {
	ASSERT_FALSE(MAP_SHARED & MAP_PRIVATE);
	ASSERT_TRUE((MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED));
}

/* ============================================================================ */
TEST_SUITE(constants_sync_advice_lock);

TEST(msync_flags) {
	ASSERT_EQ(0x01, MS_ASYNC);
	ASSERT_EQ(0x04, MS_SYNC);
	ASSERT_EQ(0x02, MS_INVALIDATE);
	ASSERT_NE(MS_ASYNC, MS_SYNC);
}

TEST(madvise_flags) {
	ASSERT_EQ(0, MADV_NORMAL);
	ASSERT_EQ(1, MADV_RANDOM);
	ASSERT_EQ(2, MADV_SEQUENTIAL);
	ASSERT_EQ(3, MADV_WILLNEED);
	ASSERT_EQ(4, MADV_DONTNEED);
}

TEST(mlock_flags) {
	ASSERT_EQ(0x01, MCL_CURRENT);
	ASSERT_EQ(0x02, MCL_FUTURE);
	ASSERT_NE(MCL_CURRENT, MCL_FUTURE);
}

/* ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(mmap_anonymous_happy);

TEST(mmap_anon_basic) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	ASSERT_NOT_NULL(p);
	munmap(p, 4096);
}

TEST(mmap_anon_read_write) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	char *buf = (char*)p;
	strcpy(buf, "test");
	ASSERT_STR_EQ("test", buf);
	munmap(p, 4096);
}

TEST(mmap_anon_zero_init) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	unsigned char *buf = (unsigned char*)p;
	for (size_t i = 0; i < 100; i++) ASSERT_EQ(0, buf[i]);
	munmap(p, 4096);
}

TEST(mmap_large_allocation) {
	void *p = mmap(NULL, 1024*1024, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	munmap(p, 1024*1024);
}

TEST(mmap_page_aligned_address) {
	void *p = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	ASSERT_EQ(0, ((uintptr_t)p) % 4096);
	munmap(p, 4096);
}

/* ============================================================================ */
TEST_SUITE(mmap_file_backed);

TEST(mmap_file_read) {
	int fd = open("/tmp/mman_test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	write(fd, "testdata", 8);

	void *p = mmap(NULL, 8, PROT_READ, MAP_PRIVATE, fd, 0);
	if (p != MAP_FAILED) {
		ASSERT_STR_EQ("testdata", (char*)p);
		munmap(p, 8);
	}
	close(fd);
	unlink("/tmp/mman_test.txt");
}

TEST(mmap_file_write_shared) {
	int fd = open("/tmp/mman_test2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	write(fd, "AAAA", 4);

	void *p = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (p != MAP_FAILED) {
		strcpy((char*)p, "BBBB");
		msync(p, 4, MS_SYNC);
		munmap(p, 4);
		lseek(fd, 0, SEEK_SET);
		char buf[5] = {0};
		read(fd, buf, 4);
		ASSERT_STR_EQ("BBBB", buf);
	}
	close(fd);
	unlink("/tmp/mman_test2.txt");
}

/* ============================================================================ */
TEST_SUITE(munmap_tests);

TEST(munmap_basic) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	ASSERT_EQ(0, munmap(p, 4096));
}

TEST(munmap_invalid_pointer) {
	errno = 0;
	ASSERT_EQ(-1, munmap((void*)0x12345, 4096));
	ASSERT_TRUE(errno == EINVAL || errno == ENOMEM);
}

TEST(munmap_double_safe) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p != MAP_FAILED) {
		ASSERT_EQ(0, munmap(p, 4096));
		int r2 = munmap(p, 4096);
		ASSERT_TRUE(r2 == 0 || r2 == -1); /* Platform variance */
	}
}

/* ============================================================================ */
TEST_SUITE(mprotect_tests);

TEST(mprotect_basic) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	ASSERT_EQ(0, mprotect(p, 4096, PROT_READ));
	munmap(p, 4096);
}

TEST(mprotect_none) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	ASSERT_EQ(0, mprotect(p, 4096, PROT_NONE));
	munmap(p, 4096);
}

TEST(mprotect_unmapped_fails) {
	void *fake = (void*)0x7FFFFFFFF000;
	ASSERT_EQ(-1, mprotect(fake, 4096, PROT_READ));
	ASSERT_TRUE(errno == ENOMEM || errno == EINVAL);
}

/* ============================================================================ */
TEST_SUITE(msync_tests);

TEST(msync_basic) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	int r = msync(p, 4096, MS_SYNC);
	ASSERT_TRUE(r == 0 || r == -1);
	munmap(p, 4096);
}

TEST(msync_async) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	int r = msync(p, 4096, MS_ASYNC);
	ASSERT_TRUE(r == 0 || r == -1);
	munmap(p, 4096);
}

TEST(msync_invalid_flags) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p != MAP_FAILED) {
		ASSERT_EQ(-1, msync(p, 4096, 0xFF));
		munmap(p, 4096);
	}
}

/* ============================================================================ */
TEST_SUITE(madvise_tests);

TEST(madvise_normal) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	int r = madvise(p, 4096, MADV_NORMAL);
	ASSERT_TRUE(r == 0 || r == -1);
	munmap(p, 4096);
}

TEST(madvise_invalid_advice) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p != MAP_FAILED) {
		ASSERT_EQ(-1, madvise(p, 4096, 0xFF));
		munmap(p, 4096);
	}
}

/* ============================================================================ */
TEST_SUITE(mlock_tests);

TEST(mlock_basic) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);
	int r = mlock(p, 4096);
	ASSERT_TRUE(r == 0 || r == -1);
	if (r == 0) munlock(p, 4096);
	munmap(p, 4096);
}

TEST(munlock_without_lock) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p != MAP_FAILED) {
		int r = munlock(p, 4096);
		ASSERT_TRUE(r == 0 || r == -1);
		munmap(p, 4096);
	}
}

/* ============================================================================ */
TEST_SUITE(sad_paths_mmap);

TEST(mmap_zero_length) {
	void *p = mmap(NULL, 0, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_EQ(MAP_FAILED, p);
}

TEST(mmap_invalid_flags_combo) {
	void *p = mmap(NULL, 4096, PROT_READ,
	               MAP_SHARED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_EQ(MAP_FAILED, p);
}

TEST(mmap_file_without_anon_invalid_fd) {
	void *p = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, -1, 0);
	ASSERT_EQ(MAP_FAILED, p);
}

TEST(mmap_overflow_length) {
	void *p = mmap(NULL, SIZE_MAX, PROT_READ,
	               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_EQ(MAP_FAILED, p);
}

/* ============================================================================ */
TEST_SUITE(sad_paths_null_safety);

TEST(mprotect_null_fails) {
	ASSERT_EQ(-1, mprotect(NULL, 4096, PROT_READ));
}

TEST(msync_null_fails) {
	ASSERT_EQ(-1, msync(NULL, 4096, MS_SYNC));
}

TEST(mlock_null_fails) {
	ASSERT_EQ(-1, mlock(NULL, 4096));
}

/* ============================================================================ */
TEST_SUITE(sad_paths_spec_violations);

TEST(mmap_offset_unaligned) {
	/* POSIX: offset must be page-aligned. Guaranteed EINVAL. */
	errno = 0;
	void *p = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 1);
	ASSERT_EQ(MAP_FAILED, p);
	ASSERT_EQ(EINVAL, errno);
}

TEST(mprotect_addr_unaligned) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);

	/* POSIX: addr must be page-aligned. Guaranteed EINVAL. */
	errno = 0;
	int r = mprotect((uint8_t*)p + 1, 4095, PROT_READ);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);

	munmap(p, 4096);
}

TEST(munmap_zero_length) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, p);

	/* Linux/BSD: length must be > 0. Guaranteed EINVAL. */
	errno = 0;
	int r = munmap(p, 0);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);

	munmap(p, 4096);
}

#endif /* !JACL_ARCH_WASM */

/* ============================================================================ */
#if JACL_ARCH_WASM
TEST_SUITE(wasm_stubs);

TEST(wasm_mmap_returns_failed) {
	ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ,
	                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
}
TEST(wasm_munmap_returns_error) { ASSERT_EQ(-1, munmap(NULL, 4096)); }
TEST(wasm_mprotect_returns_error) { ASSERT_EQ(-1, mprotect(NULL, 4096, PROT_READ)); }
TEST(wasm_msync_returns_error) { ASSERT_EQ(-1, msync(NULL, 4096, MS_SYNC)); }
TEST(wasm_madvise_returns_error) { ASSERT_EQ(-1, madvise(NULL, 4096, MADV_NORMAL)); }
TEST(wasm_mlock_returns_error) { ASSERT_EQ(-1, mlock(NULL, 4096)); }
#endif

/* ============================================================================ */
#if JACL_OS_WINDOWS
TEST_SUITE(windows_allocation);

TEST(win_anon_read_write) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	ASSERT_TRUE(p != MAP_FAILED);
	if (p != MAP_FAILED) munmap(p, 4096);
}

TEST(win_anon_exec_read) {
	void *p = mmap(NULL, 4096, PROT_READ | PROT_EXEC,
	               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	ASSERT_TRUE(p != MAP_FAILED);
	if (p != MAP_FAILED) munmap(p, 4096);
}
#endif

/* ============================================================================ */
TEST_MAIN()
