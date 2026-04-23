/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(sys/socket.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_address_families) {
	ASSERT_EQ(0, AF_UNSPEC);
	ASSERT_EQ(1, AF_UNIX);
	ASSERT_EQ(AF_UNIX, AF_LOCAL);
	ASSERT_EQ(2, AF_INET);
	ASSERT_EQ(10, AF_INET6);
}

TEST(constants_sock_types) {
	ASSERT_EQ(1, SOCK_STREAM);
	ASSERT_EQ(2, SOCK_DGRAM);
	ASSERT_EQ(3, SOCK_RAW);
	ASSERT_EQ(4, SOCK_RDM);
	ASSERT_EQ(5, SOCK_SEQPACKET);
}

TEST(constants_sock_options_core) {
	ASSERT_EQ(1, SOL_SOCKET);
	ASSERT_EQ(1, SO_DEBUG);
	ASSERT_EQ(2, SO_REUSEADDR);
	ASSERT_EQ(3, SO_TYPE);
	ASSERT_EQ(4, SO_ERROR);
	ASSERT_EQ(5, SO_DONTROUTE);
	ASSERT_EQ(6, SO_BROADCAST);
	ASSERT_EQ(7, SO_SNDBUF);
	ASSERT_EQ(8, SO_RCVBUF);
	ASSERT_EQ(9, SO_KEEPALIVE);
	ASSERT_EQ(10, SO_OOBINLINE);
	ASSERT_EQ(13, SO_LINGER);
	ASSERT_EQ(15, SO_REUSEPORT);
}

TEST(constants_sock_options_cred) {
	ASSERT_EQ(16, SO_PASSCRED);
	ASSERT_EQ(17, SO_PEERCRED);
}

TEST(constants_sock_options_lowat) {
	ASSERT_EQ(18, SO_RCVLOWAT);
	ASSERT_EQ(19, SO_SNDLOWAT);
}

TEST(constants_sock_options_timeout) {
	ASSERT_TRUE(SO_RCVTIMEO > 0);
	ASSERT_TRUE(SO_SNDTIMEO > 0);

#if JACL_HAS_BSD
	ASSERT_EQ(0x1006, SO_RCVTIMEO);
	ASSERT_EQ(0x1005, SO_SNDTIMEO);
#else
	ASSERT_EQ(20, SO_RCVTIMEO);
	ASSERT_EQ(21, SO_SNDTIMEO);
#endif
}

TEST(constants_sock_options_security_filter) {
	ASSERT_EQ(22, SO_SECURITY_AUTHENTICATION);
	ASSERT_EQ(23, SO_SECURITY_ENCRYPTION_TRANSPORT);
	ASSERT_EQ(24, SO_SECURITY_ENCRYPTION_NETWORK);
	ASSERT_EQ(25, SO_BINDTODEVICE);
	ASSERT_EQ(26, SO_ATTACH_FILTER);
	ASSERT_EQ(27, SO_DETACH_FILTER);
	ASSERT_EQ(SO_ATTACH_FILTER, SO_GET_FILTER);
}

TEST(constants_sock_options_misc) {
	ASSERT_EQ(28, SO_PEERNAME);
	ASSERT_EQ(29, SO_TIMESTAMP);
	ASSERT_EQ(30, SO_ACCEPTCONN);
}

TEST(constants_sock_messages) {
	ASSERT_EQ(0x01, MSG_OOB);
	ASSERT_EQ(0x02, MSG_PEEK);
	ASSERT_EQ(0x04, MSG_DONTROUTE);
	ASSERT_EQ(0x08, MSG_CTRUNC);
	ASSERT_EQ(0x20, MSG_TRUNC);
	ASSERT_EQ(0x40, MSG_DONTWAIT);
	ASSERT_EQ(0x80, MSG_EOR);
	ASSERT_EQ(0x100, MSG_WAITALL);
	ASSERT_EQ(0x4000, MSG_NOSIGNAL);
}

TEST(constants_shutdown) {
	ASSERT_EQ(0, SHUT_RD);
	ASSERT_EQ(1, SHUT_WR);
	ASSERT_EQ(2, SHUT_RDWR);
}

/* ============================================================================ */
TEST_SUITE(sockaddr);

TEST(sockaddr_size) {
	ASSERT_EQ(16, sizeof(struct sockaddr));
}

TEST(sockaddr_family_offset) {
	ASSERT_OFFSET(struct sockaddr, sa_family, 0);
}

TEST(sockaddr_data_offset) {
	ASSERT_OFFSET(struct sockaddr, sa_data, 2);
}

TEST(sockaddr_assignment) {
	struct sockaddr sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_family = AF_INET;
	ASSERT_EQ(AF_INET, sa.sa_family);
}

/* ============================================================================ */
TEST_SUITE(msghdr);

TEST(msghdr_size) {
	ASSERT_TRUE(sizeof(struct msghdr) >= (sizeof(void*) * 3 + sizeof(int) * 3 + sizeof(size_t)));
}

TEST(msghdr_field_names) {
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = NULL;
	msg.msg_iovlen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	ASSERT_NULL(msg.msg_name);
	ASSERT_EQ(0, msg.msg_namelen);
	ASSERT_NULL(msg.msg_iov);
	ASSERT_EQ(0, msg.msg_iovlen);
	ASSERT_NULL(msg.msg_control);
	ASSERT_EQ(0, msg.msg_controllen);
	ASSERT_EQ(0, msg.msg_flags);
}

TEST(msghdr_offsets) {
	ASSERT_TRUE(offsetof(struct msghdr, msg_name) == 0);
	ASSERT_TRUE(offsetof(struct msghdr, msg_namelen) > 0);
	ASSERT_TRUE(offsetof(struct msghdr, msg_iov) > offsetof(struct msghdr, msg_namelen));
}

/* ============================================================================ */
TEST_SUITE(cmsghdr);

TEST(cmsghdr_size) {
	ASSERT_TRUE(sizeof(struct cmsghdr) >= (sizeof(socklen_t) + sizeof(int) * 2));
}

TEST(cmsghdr_field_names) {
	struct cmsghdr cmsg;
	memset(&cmsg, 0, sizeof(cmsg));

	cmsg.cmsg_len = 0;
	cmsg.cmsg_level = 0;
	cmsg.cmsg_type = 0;

	ASSERT_EQ(0, cmsg.cmsg_len);
	ASSERT_EQ(0, cmsg.cmsg_level);
	ASSERT_EQ(0, cmsg.cmsg_type);
}

TEST(cmsghdr_offsets) {
	ASSERT_EQ(0, offsetof(struct cmsghdr, cmsg_len));
	ASSERT_TRUE(offsetof(struct cmsghdr, cmsg_level) > 0);
	ASSERT_TRUE(offsetof(struct cmsghdr, cmsg_type) > offsetof(struct cmsghdr, cmsg_level));
}

/* ============================================================================ */
TEST_SUITE(cmsg_align);

TEST(cmsg_align_zero) {
	ASSERT_EQ(0, CMSG_ALIGN(0));
}

TEST(cmsg_align_one) {
	ASSERT_EQ(sizeof(size_t), CMSG_ALIGN(1));
}

TEST(cmsg_align_exact) {
	ASSERT_EQ(sizeof(size_t), CMSG_ALIGN(sizeof(size_t)));
}

TEST(cmsg_align_boundary) {
	size_t s = sizeof(size_t);
	ASSERT_EQ(s * 2, CMSG_ALIGN(s + 1));
}

TEST(cmsg_align_large) {
	size_t s = sizeof(size_t);
	size_t val = 100;
	size_t expected = ((val + s - 1) / s) * s;
	ASSERT_EQ(expected, CMSG_ALIGN(val));
}

/* ============================================================================ */
TEST_SUITE(cmsg_space);

TEST(cmsg_space_zero_data) {
	size_t space = CMSG_SPACE(0);
	ASSERT_TRUE(space >= CMSG_ALIGN(sizeof(struct cmsghdr)));
	ASSERT_TRUE(space % sizeof(size_t) == 0);
}

TEST(cmsg_space_small_data) {
	size_t space = CMSG_SPACE(1);
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(1);
	ASSERT_EQ(expected, space);
}

TEST(cmsg_space_large_data) {
	size_t space = CMSG_SPACE(1024);
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(1024);
	ASSERT_EQ(expected, space);
}

/* ============================================================================ */
TEST_SUITE(cmsg_len);

TEST(cmsg_len_zero_data) {
	size_t len = CMSG_LEN(0);
	ASSERT_TRUE(len >= CMSG_ALIGN(sizeof(struct cmsghdr)));
}

TEST(cmsg_len_small_data) {
	size_t len = CMSG_LEN(5);
	/* CMSG_LEN adds header align + raw data len (no data align) */
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + 5;
	ASSERT_EQ(expected, len);
}

TEST(cmsg_len_vs_space) {
	ASSERT_TRUE(CMSG_SPACE(10) >= CMSG_LEN(10));
	/* They should differ if data size is not aligned */
	if (10 % sizeof(size_t) != 0) {
		ASSERT_TRUE(CMSG_SPACE(10) > CMSG_LEN(10));
	}
}

/* ============================================================================ */
TEST_SUITE(cmsg_data);

TEST(cmsg_data_ptr_valid) {
	struct cmsghdr cmsg;
	cmsg.cmsg_len = CMSG_LEN(10);

	unsigned char *data = CMSG_DATA(&cmsg);
	unsigned char *expected = ((unsigned char *)&cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr));

	ASSERT_PTR_EQ(data, expected);
	ASSERT_TRUE(data > (unsigned char *)&cmsg);
}

TEST(cmsg_data_offset_consistency) {
	/* Verify offset is constant regardless of cmsg_len value */
	struct cmsghdr cmsg1, cmsg2;
	cmsg1.cmsg_len = CMSG_LEN(0);
	cmsg2.cmsg_len = CMSG_LEN(100);

	unsigned char *d1 = CMSG_DATA(&cmsg1);
	unsigned char *d2 = CMSG_DATA(&cmsg2);

	/* Both should point to same relative offset from their own start */
	ASSERT_EQ((uintptr_t)d1 - (uintptr_t)&cmsg1, (uintptr_t)d2 - (uintptr_t)&cmsg2);
}

TEST(cmsg_data_writable) {
	/* Allocate header + payload space */
	char buffer[CMSG_SPACE(8)] = {0};
	struct cmsghdr *cmsg = (struct cmsghdr *)buffer;
	cmsg->cmsg_len = CMSG_LEN(8);
	unsigned char *data = CMSG_DATA(cmsg);

	/* Ensure we can write to the data pointer without crashing */
	data[0] = 0xAA;
	data[1] = 0xBB;
	ASSERT_EQ(0xAA, data[0]);
	ASSERT_EQ(0xBB, data[1]);
}

/* REMOVED: cmsg_data_alignment (Absolute alignment depends on stack, not macro) */

/* ============================================================================ */
TEST_SUITE(cmsg_firsthdr);

TEST(cmsg_firsthdr_null_control) {
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(cmsg_firsthdr_zero_len) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = 0;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(cmsg_firsthdr_too_small) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = sizeof(struct cmsghdr) - 1;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(cmsg_firsthdr_exact_min) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = CMSG_ALIGN(sizeof(struct cmsghdr));
	ASSERT_PTR_EQ(CMSG_FIRSTHDR(&msg), &cmsg);
}

TEST(cmsg_firsthdr_larger_buffer) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	char buffer[128];
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = buffer;
	msg.msg_controllen = sizeof(buffer);

	/* Should return pointer to start of buffer casted */
	ASSERT_PTR_EQ(CMSG_FIRSTHDR(&msg), (struct cmsghdr *)buffer);
}

/* ============================================================================ */
TEST_SUITE(cmsg_nxthdr);

TEST(cmsg_nxthdr_end_of_buffer) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = CMSG_SPACE(0); /* Exactly one header */

	cmsg.cmsg_len = CMSG_LEN(0);

	/* No room for next */
	ASSERT_NULL(CMSG_NXTHDR(&msg, &cmsg));
}

TEST(cmsg_nxthdr_exactly_fits_two) {
	struct msghdr msg;
	char buffer[256];
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = buffer;

	size_t h1 = CMSG_SPACE(0);
	size_t h2 = CMSG_SPACE(0);
	msg.msg_controllen = h1 + h2;

	struct cmsghdr *c1 = (struct cmsghdr *)buffer;
	c1->cmsg_len = CMSG_LEN(0);

	struct cmsghdr *c2 = CMSG_NXTHDR(&msg, c1);
	ASSERT_NOT_NULL(c2);
	ASSERT_PTR_EQ(c2, buffer + h1);
}

TEST(cmsg_nxthdr_truncated_middle) {
	struct msghdr msg;
	char buffer[256];
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = buffer;
	msg.msg_controllen = 100; /* Arbitrary limit */

	struct cmsghdr *c1 = (struct cmsghdr *)buffer;
	/* Claim length that exceeds buffer */
	c1->cmsg_len = CMSG_LEN(200);

	/* Should return NULL because next would be out of bounds */
	ASSERT_NULL(CMSG_NXTHDR(&msg, c1));
}

TEST(cmsg_nxthdr_chain_three) {
	struct msghdr msg;
	char buffer[512];
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = buffer;

	size_t hs = CMSG_SPACE(10);
	msg.msg_controllen = hs * 3;

	struct cmsghdr *c1 = (struct cmsghdr *)buffer;
	c1->cmsg_len = CMSG_LEN(10);

	struct cmsghdr *c2 = CMSG_NXTHDR(&msg, c1);
	ASSERT_NOT_NULL(c2);
	c2->cmsg_len = CMSG_LEN(10);

	struct cmsghdr *c3 = CMSG_NXTHDR(&msg, c2);
	ASSERT_NOT_NULL(c3);

	struct cmsghdr *c4 = CMSG_NXTHDR(&msg, c3);
	ASSERT_NULL(c4); /* End of chain */
}

/* ============================================================================ */
TEST_SUITE(socket);

TEST(socket_invalid_family) {
	int fd = socket(999, SOCK_STREAM, 0);
	ASSERT_EQ(-1, fd);
	ASSERT_TRUE(errno == EAFNOSUPPORT || errno == EINVAL || errno == ENOSYS);
}

TEST(socket_invalid_type) {
	int fd = socket(AF_INET, 999, 0);
	ASSERT_EQ(-1, fd);
	ASSERT_TRUE(errno == EINVAL || errno == ENOSYS || errno == EPROTONOSUPPORT);
}

TEST(socket_valid_unix) {
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd >= 0) {
		close(fd);
	} else {
		ASSERT_TRUE(errno == EMFILE || errno == ENFILE || errno == EAFNOSUPPORT || errno == ENOSYS);
	}
}

TEST(socket_valid_udp) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd >= 0) {
		close(fd);
	} else {
		ASSERT_TRUE(errno == EMFILE || errno == ENFILE || errno == EAFNOSUPPORT || errno == ENOSYS);
	}
}

/* ============================================================================ */
TEST_SUITE(shutdown);

TEST(shutdown_invalid_fd) {
	int ret = shutdown(99999, SHUT_RD);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOTSOCK || errno == ENOSYS);
}

TEST(shutdown_constants_valid) {
	ASSERT_EQ(0, SHUT_RD);
	ASSERT_EQ(1, SHUT_WR);
	ASSERT_EQ(2, SHUT_RDWR);
}

TEST_MAIN()

#else

int main(void) {
	printf("sys/socket.h requires POSIX\n");
	return 0;
}

#endif
