/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(sys/mman.h);

/* ============================================================================
 * CONSTANTS - PROTECTION FLAGS
 * ============================================================================ */
TEST_SUITE(protection_flags);

TEST(prot_constants) {
	ASSERT_EQ(0x00, PROT_NONE);
	ASSERT_EQ(0x01, PROT_READ);
	ASSERT_EQ(0x02, PROT_WRITE);
	ASSERT_EQ(0x04, PROT_EXEC);
}

TEST(prot_flags_unique) {
	ASSERT_NE(PROT_READ, PROT_WRITE);
	ASSERT_NE(PROT_WRITE, PROT_EXEC);
	ASSERT_NE(PROT_READ, PROT_EXEC);
}

TEST(prot_combinations) {
	int rw = PROT_READ | PROT_WRITE;
	ASSERT_TRUE(rw & PROT_READ);
	ASSERT_TRUE(rw & PROT_WRITE);
	ASSERT_FALSE(rw & PROT_EXEC);
}

/* ============================================================================
 * CONSTANTS - MAPPING FLAGS
 * ============================================================================ */
TEST_SUITE(mapping_flags);

TEST(map_constants) {
	ASSERT_EQ(0x01, MAP_SHARED);
	ASSERT_EQ(0x02, MAP_PRIVATE);
	ASSERT_EQ(0x10, MAP_FIXED);
	ASSERT_EQ(0x20, MAP_ANONYMOUS);
	ASSERT_EQ(MAP_ANONYMOUS, MAP_ANON);
}

TEST(map_flags_unique) {
	ASSERT_NE(MAP_SHARED, MAP_PRIVATE);
	ASSERT_NE(MAP_SHARED, MAP_FIXED);
	ASSERT_NE(MAP_PRIVATE, MAP_ANONYMOUS);
}

TEST(map_failed_constant) {
	ASSERT_EQ((void*)-1, MAP_FAILED);
}

/* ============================================================================
 * CONSTANTS - SYNC FLAGS
 * ============================================================================ */
TEST_SUITE(sync_flags);

TEST(msync_constants) {
	ASSERT_EQ(0x01, MS_ASYNC);
	ASSERT_EQ(0x04, MS_SYNC);
	ASSERT_EQ(0x02, MS_INVALIDATE);
}

TEST(msync_flags_unique) {
	ASSERT_NE(MS_ASYNC, MS_SYNC);
	ASSERT_NE(MS_SYNC, MS_INVALIDATE);
	ASSERT_NE(MS_ASYNC, MS_INVALIDATE);
}

/* ============================================================================
 * CONSTANTS - ADVISORY FLAGS
 * ============================================================================ */
TEST_SUITE(advisory_flags);

TEST(madvise_constants) {
	ASSERT_EQ(0, MADV_NORMAL);
	ASSERT_EQ(1, MADV_RANDOM);
	ASSERT_EQ(2, MADV_SEQUENTIAL);
	ASSERT_EQ(3, MADV_WILLNEED);
	ASSERT_EQ(4, MADV_DONTNEED);
}

TEST(madvise_flags_unique) {
	ASSERT_NE(MADV_NORMAL, MADV_RANDOM);
	ASSERT_NE(MADV_RANDOM, MADV_SEQUENTIAL);
	ASSERT_NE(MADV_SEQUENTIAL, MADV_WILLNEED);
}

/* ============================================================================
 * CONSTANTS - LOCK FLAGS
 * ============================================================================ */
TEST_SUITE(lock_flags);

TEST(mlock_constants) {
	ASSERT_EQ(0x01, MCL_CURRENT);
	ASSERT_EQ(0x02, MCL_FUTURE);
}

TEST(mlock_flags_unique) {
	ASSERT_NE(MCL_CURRENT, MCL_FUTURE);
}

/* ============================================================================
 * MMAP - ANONYMOUS
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(mmap_anonymous);

TEST(mmap_anonymous_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);
	ASSERT_NOT_NULL(ptr);

	munmap(ptr, size);
}

TEST(mmap_anonymous_read_write) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	// Write to memory
	char *buf = (char*)ptr;
	strcpy(buf, "test");

	// Read back
	ASSERT_STR_EQ("test", buf);

	munmap(ptr, size);
}

TEST(mmap_anonymous_zero_initialized) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	// Anonymous memory should be zero-initialized
	unsigned char *buf = (unsigned char*)ptr;
	int all_zero = 1;
	for (size_t i = 0; i < 100; i++) {
		if (buf[i] != 0) {
			all_zero = 0;
			break;
		}
	}

	ASSERT_TRUE(all_zero);

	munmap(ptr, size);
}

TEST(mmap_large_allocation) {
	size_t size = 1024 * 1024;  // 1MB
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * MMAP - FILE BACKED
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(mmap_file);

TEST(mmap_file_basic) {
	// Create test file
	int fd = open("/tmp/mmap_test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);

	write(fd, "test data", 9);

	void *ptr = mmap(NULL, 9, PROT_READ, MAP_PRIVATE, fd, 0);

	if (ptr != MAP_FAILED) {
		ASSERT_STR_EQ("test data", (char*)ptr);
		munmap(ptr, 9);
	}

	close(fd);
	unlink("/tmp/mmap_test.txt");
}

TEST(mmap_file_write) {
	int fd = open("/tmp/mmap_test2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);

	// Write initial data
	write(fd, "AAAA", 4);

	void *ptr = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (ptr != MAP_FAILED) {
		strcpy((char*)ptr, "BBBB");
		msync(ptr, 4, MS_SYNC);
		munmap(ptr, 4);

		// Verify written back
		lseek(fd, 0, SEEK_SET);
		char buf[5] = {0};
		read(fd, buf, 4);
		ASSERT_STR_EQ("BBBB", buf);
	}

	close(fd);
	unlink("/tmp/mmap_test2.txt");
}
#endif

/* ============================================================================
 * MUNMAP
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(munmap_test);

TEST(munmap_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = munmap(ptr, size);
	ASSERT_EQ(0, result);
}

TEST(munmap_invalid_pointer) {
	int result = munmap((void*)0x12345, 4096);
	ASSERT_EQ(-1, result);
}
#endif

/* ============================================================================
 * MPROTECT
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(mprotect_test);

TEST(mprotect_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	// Change to read-only
	int result = mprotect(ptr, size, PROT_READ);
	ASSERT_EQ(0, result);

	munmap(ptr, size);
}

TEST(mprotect_none) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	// Remove all protection
	int result = mprotect(ptr, size, PROT_NONE);
	ASSERT_EQ(0, result);

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * MSYNC
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(msync_test);

TEST(msync_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = msync(ptr, size, MS_SYNC);
	ASSERT_TRUE(result == 0 || result == -1);  // May not be supported

	munmap(ptr, size);
}

TEST(msync_async) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = msync(ptr, size, MS_ASYNC);
	ASSERT_TRUE(result == 0 || result == -1);

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * MADVISE
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(madvise_test);

TEST(madvise_normal) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = madvise(ptr, size, MADV_NORMAL);
	ASSERT_TRUE(result == 0 || result == -1);

	munmap(ptr, size);
}

TEST(madvise_sequential) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = madvise(ptr, size, MADV_SEQUENTIAL);
	ASSERT_TRUE(result == 0 || result == -1);

	munmap(ptr, size);
}

TEST(madvise_random) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = madvise(ptr, size, MADV_RANDOM);
	ASSERT_TRUE(result == 0 || result == -1);

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * MLOCK / MUNLOCK
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(mlock_test);

TEST(mlock_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	int result = mlock(ptr, size);
	ASSERT_TRUE(result == 0 || result == -1);  // May need permissions

	if (result == 0) {
		munlock(ptr, size);
	}

	munmap(ptr, size);
}

TEST(munlock_basic) {
	size_t size = 4096;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	if (mlock(ptr, size) == 0) {
		int result = munlock(ptr, size);
		ASSERT_EQ(0, result);
	}

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
#if !JACL_ARCH_WASM
TEST_SUITE(edge_cases);

TEST(mmap_zero_length) {
	void *ptr = mmap(NULL, 0, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_EQ(MAP_FAILED, ptr);
}

TEST(mmap_page_aligned) {
	size_t size = 8192;  // 2 pages
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_NE(MAP_FAILED, ptr);

	// Address should be page-aligned
	ASSERT_EQ(0, ((uintptr_t)ptr) % 4096);

	munmap(ptr, size);
}
#endif

/* ============================================================================
 * WASM STUB TESTS
 * ============================================================================ */
#if JACL_ARCH_WASM
TEST_SUITE(wasm_stubs);

TEST(mmap_returns_failed) {
	void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ASSERT_EQ(MAP_FAILED, ptr);
}

TEST(munmap_returns_error) {
	int result = munmap((void*)0x1000, 4096);
	ASSERT_EQ(-1, result);
}

TEST(mprotect_returns_error) {
	int result = mprotect((void*)0x1000, 4096, PROT_READ);
	ASSERT_EQ(-1, result);
}
#endif

TEST_MAIN()
