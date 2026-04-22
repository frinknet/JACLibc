/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <sys/uio.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(sys/uio.h);

TEST_SUITE(iovec_struct);

TEST(iovec_layout) {
	struct iovec iov;
	char buf[16] = "test";
	iov.iov_base = buf;
	iov.iov_len = 4;

	ASSERT_PTR_EQ(buf, iov.iov_base);
	ASSERT_EQ(4, iov.iov_len);
}

TEST(iovec_size) {
	ASSERT_EQ(sizeof(void *) + sizeof(size_t), sizeof(struct iovec));
}

TEST(iovec_array) {
	struct iovec vec[3] = {
		{ .iov_base = "a", .iov_len = 1 },
		{ .iov_base = "bb", .iov_len = 2 },
		{ .iov_base = "ccc", .iov_len = 3 }
	};

	ASSERT_EQ(1, vec[0].iov_len);
	ASSERT_EQ(2, vec[1].iov_len);
	ASSERT_EQ(3, vec[2].iov_len);
	ASSERT_EQ(0, memcmp(vec[0].iov_base, "a", 1));
	ASSERT_EQ(0, memcmp(vec[2].iov_base, "ccc", 3));
}

TEST(iovec_zero_length) {
	struct iovec v = { .iov_base = (void *)0xDEAD, .iov_len = 0 };
	ASSERT_NOT_NULL(v.iov_base);
	ASSERT_EQ(0, v.iov_len);
}

TEST(iovec_null_base_nonzero_len) {
	struct iovec v = { .iov_base = NULL, .iov_len = 1024 };
	ASSERT_NULL(v.iov_base);
	ASSERT_EQ(1024, v.iov_len);
}

TEST(iovec_large_array_overflow) {
	/* Verify struct layout doesn't break on massive arrays */
	struct iovec arr[1000] = {0};
	ASSERT_EQ(sizeof(struct iovec) * 1000, sizeof(arr));
}

TEST(iovec_field_alignment) {
	struct iovec v;
	/* void* and size_t must align to native word boundaries */
	ASSERT_EQ(0, (uintptr_t)&v.iov_base % alignof(void *));
	ASSERT_EQ(0, (uintptr_t)&v.iov_len  % alignof(size_t));
}

#ifdef IOV_MAX
TEST(iovec_max_constant) {
	/* POSIX mandates at least 16. Linux/glibc uses 1024. */
	ASSERT_GE(IOV_MAX, 16);
	ASSERT_LE(IOV_MAX, 1073741824); /* Sanity cap */
}
#endif

TEST_MAIN()

#else

int main(void) { printf("sys/uio.h requires POSIX\n"); return 0; }

#endif
