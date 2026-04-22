/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>

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

TEST(msghdr_exists) {
	struct msghdr msg;
	(void)msg;
	ASSERT_TRUE(1);
}

TEST(msghdr_size) {
	/* Size varies by arch, but must be at least large enough for all pointers + ints */
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
	/* Verify standard field order/offsets roughly match expectations */
	ASSERT_TRUE(offsetof(struct msghdr, msg_name) == 0);
	/* Note: Exact offsets depend on pointer alignment, but names must exist */
	ASSERT_TRUE(offsetof(struct msghdr, msg_namelen) > 0);
	ASSERT_TRUE(offsetof(struct msghdr, msg_iov) > offsetof(struct msghdr, msg_namelen));
}

/* ============================================================================ */

TEST_SUITE(cmsghdr);

TEST(cmsghdr_exists) {
	struct cmsghdr cmsg;
	(void)cmsg;
	ASSERT_TRUE(1);
}

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

TEST_SUITE(cmsg_space);

TEST(cmsg_space_len) {
	size_t space = CMSG_SPACE(0);
	size_t len = CMSG_LEN(0);

	ASSERT_TRUE(space >= sizeof(struct cmsghdr));
	ASSERT_TRUE(len >= sizeof(struct cmsghdr));
	ASSERT_TRUE(space >= len);
}

TEST(cmsg_space_alignment) {
	size_t space = CMSG_SPACE(10);
	/* Should be aligned to sizeof(size_t) or similar */
	ASSERT_TRUE(space % sizeof(size_t) == 0);
}

/* ============================================================================ */

TEST_SUITE(cmsg_data);

TEST(cmsg_data_ptr) {
	struct cmsghdr cmsg;
	cmsg.cmsg_len = CMSG_LEN(10);

	unsigned char *data = CMSG_DATA(&cmsg);
	unsigned char *expected = ((unsigned char *)&cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr));

	ASSERT_PTR_EQ(data, expected);
	ASSERT_TRUE(data > (unsigned char *)&cmsg);
}

/* ============================================================================ */

TEST_SUITE(cmsg_fisthdr);

TEST(cmsg_firsthdr_null) {
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

/* ============================================================================ */

TEST_SUITE(cmsg_nexthdr);

TEST(cmsg_nxthdr_safety) {
	/* Basic sanity check that macro compiles and doesn't crash on empty */
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	memset(&cmsg, 0, sizeof(cmsg));

	msg.msg_control = &cmsg;
	msg.msg_controllen = sizeof(cmsg);

	/* Might return NULL if len is too small, but shouldn't crash */
	(void)CMSG_NXTHDR(&msg, &cmsg);
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
	/* Just ensuring constants are usable */
	ASSERT_EQ(0, SHUT_RD);
	ASSERT_EQ(1, SHUT_WR);
	ASSERT_EQ(2, SHUT_RDWR);
}

/* ============================================================================ */

TEST_MAIN()

#else

int main(void) {
	printf("sys/socket.h requires POSIX\n");
	return 0;
}

#endif
