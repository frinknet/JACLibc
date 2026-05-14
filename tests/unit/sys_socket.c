/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

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

TEST(constants_sock_flags) {
	ASSERT_EQ(04000, SOCK_NONBLOCK);
	ASSERT_EQ(02000000, SOCK_CLOEXEC);
}

TEST(constants_protocol_levels) {
	ASSERT_EQ(1, SOL_SOCKET);
	ASSERT_EQ(0, SOL_IP);
	ASSERT_EQ(41, SOL_IPV6);
	ASSERT_EQ(6, SOL_TCP);
	ASSERT_EQ(17, SOL_UDP);
}

TEST(constants_sock_options) {
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
	ASSERT_EQ(16, SO_PASSCRED);
	ASSERT_EQ(17, SO_PEERCRED);
	ASSERT_EQ(18, SO_RCVLOWAT);
	ASSERT_EQ(19, SO_SNDLOWAT);
	ASSERT_EQ(20, SO_RCVTIMEO);
	ASSERT_EQ(21, SO_SNDTIMEO);
}

TEST(constants_message_flags) {
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

TEST_SUITE(sockaddr_storage);

TEST(sockaddr_storage_minimum_size) {
	ASSERT_TRUE(sizeof(struct sockaddr_storage) >= 128);
}

TEST(sockaddr_storage_alignment) {
	ASSERT_EQ(0, (uintptr_t)&((struct sockaddr_storage *)0)->ss_family % _SS_ALIGNSIZE);
}

TEST(sockaddr_storage_family_offset) {
	ASSERT_EQ(0, offsetof(struct sockaddr_storage, ss_family));
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

TEST_SUITE(linger);

TEST(linger_size) {
	ASSERT_TRUE(sizeof(struct linger) >= (sizeof(int) * 2));
}

TEST(linger_field_names) {
	struct linger lng;
	memset(&lng, 0, sizeof(lng));
	lng.l_onoff = 1;
	lng.l_linger = 30;
	ASSERT_EQ(1, lng.l_onoff);
	ASSERT_EQ(30, lng.l_linger);
}

/* ============================================================================ */

TEST_SUITE(CMSG_ALIGN);

TEST(CMSG_ALIGN_zero) {
	ASSERT_EQ(0, CMSG_ALIGN(0));
}

TEST(CMSG_ALIGN_one) {
	ASSERT_EQ(sizeof(size_t), CMSG_ALIGN(1));
}

TEST(CMSG_ALIGN_exact) {
	ASSERT_EQ(sizeof(size_t), CMSG_ALIGN(sizeof(size_t)));
}

TEST(CMSG_ALIGN_boundary) {
	size_t s = sizeof(size_t);
	ASSERT_EQ(s * 2, CMSG_ALIGN(s + 1));
}

TEST(CMSG_ALIGN_large) {
	size_t s = sizeof(size_t);
	size_t val = 100;
	size_t expected = ((val + s - 1) / s) * s;
	ASSERT_EQ(expected, CMSG_ALIGN(val));
}

/* ============================================================================ */

TEST_SUITE(CMSG_SPACE);

TEST(CMSG_SPACE_zero_data) {
	size_t space = CMSG_SPACE(0);
	ASSERT_TRUE(space >= CMSG_ALIGN(sizeof(struct cmsghdr)));
	ASSERT_TRUE(space % sizeof(size_t) == 0);
}

TEST(CMSG_SPACE_small_data) {
	size_t space = CMSG_SPACE(1);
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(1);
	ASSERT_EQ(expected, space);
}

TEST(CMSG_SPACE_large_data) {
	size_t space = CMSG_SPACE(1024);
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(1024);
	ASSERT_EQ(expected, space);
}

/* ============================================================================ */

TEST_SUITE(CMSG_LEN);

TEST(CMSG_LEN_zero_data) {
	size_t len = CMSG_LEN(0);
	ASSERT_TRUE(len >= CMSG_ALIGN(sizeof(struct cmsghdr)));
}

TEST(CMSG_LEN_small_data) {
	size_t len = CMSG_LEN(5);
	size_t expected = CMSG_ALIGN(sizeof(struct cmsghdr)) + 5;
	ASSERT_EQ(expected, len);
}

TEST(CMSG_LEN_vs_space) {
	ASSERT_TRUE(CMSG_SPACE(10) >= CMSG_LEN(10));
	if (10 % sizeof(size_t) != 0) {
		ASSERT_TRUE(CMSG_SPACE(10) > CMSG_LEN(10));
	}
}

/* ============================================================================ */

TEST_SUITE(CMSG_FIRSTHDR);

TEST(CMSG_FIRSTHDR_null_control) {
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(CMSG_FIRSTHDR_zero_len) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = 0;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(CMSG_FIRSTHDR_too_small) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = sizeof(struct cmsghdr) - 1;
	ASSERT_NULL(CMSG_FIRSTHDR(&msg));
}

TEST(CMSG_FIRSTHDR_exact_min) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = CMSG_ALIGN(sizeof(struct cmsghdr));
	ASSERT_PTR_EQ(CMSG_FIRSTHDR(&msg), &cmsg);
}

/* ============================================================================ */

TEST_SUITE(CMSG_NXTHDR);

TEST(CMSG_NXTHDR_end_of_buffer) {
	struct msghdr msg;
	struct cmsghdr cmsg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = &cmsg;
	msg.msg_controllen = CMSG_SPACE(0);
	cmsg.cmsg_len = CMSG_LEN(0);
	ASSERT_NULL(CMSG_NXTHDR(&msg, &cmsg));
}

TEST(CMSG_NXTHDR_chain_three) {
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
	ASSERT_NULL(c4);
}

/* ============================================================================ */

TEST_SUITE(CMSG_DATA);

TEST(CMSG_DATA_ptr_valid) {
	struct cmsghdr cmsg;
	cmsg.cmsg_len = CMSG_LEN(10);
	unsigned char *data = CMSG_DATA(&cmsg);
	unsigned char *expected = ((unsigned char *)&cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr));
	ASSERT_PTR_EQ(data, expected);
	ASSERT_TRUE(data > (unsigned char *)&cmsg);
}

TEST(CMSG_DATA_offset_consistency) {
	struct cmsghdr cmsg1, cmsg2;
	cmsg1.cmsg_len = CMSG_LEN(0);
	cmsg2.cmsg_len = CMSG_LEN(100);
	unsigned char *d1 = CMSG_DATA(&cmsg1);
	unsigned char *d2 = CMSG_DATA(&cmsg2);
	ASSERT_EQ((uintptr_t)d1 - (uintptr_t)&cmsg1, (uintptr_t)d2 - (uintptr_t)&cmsg2);
}

TEST(CMSG_DATA_writable) {
	char buffer[CMSG_SPACE(8)] = {0};
	struct cmsghdr *cmsg = (struct cmsghdr *)buffer;
	cmsg->cmsg_len = CMSG_LEN(8);
	unsigned char *data = CMSG_DATA(cmsg);
	data[0] = 0xAA;
	data[1] = 0xBB;
	ASSERT_EQ(0xAA, data[0]);
	ASSERT_EQ(0xBB, data[1]);
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

TEST_SUITE(bind);

TEST(bind_null_addr) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = bind(fd, NULL, 0);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(bind_invalid_fd) {
	struct sockaddr sa = {0};
	int ret = bind(-1, &sa, sizeof(sa));
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOTSOCK || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(send);

TEST(send_zero_length) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		uint8_t buf[1] = {0};
		ssize_t ret = send(fd, buf, 0, 0);
		ASSERT_EQ(0, ret);
		close(fd);
	}
}

TEST(send_invalid_fd) {
	uint8_t buf[1] = {0};
	ssize_t ret = send(-1, buf, 1, 0);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(recv);

TEST(recv_zero_length) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		uint8_t buf[1] = {0};
		ssize_t ret = recv(fd, buf, 0, 0);
		ASSERT_EQ(0, ret);
		close(fd);
	}
}

TEST(recv_invalid_fd) {
	uint8_t buf[1] = {0};
	ssize_t ret = recv(-1, buf, 1, 0);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(sendto);

TEST(sendto_zero_length) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd >= 0) {
		uint8_t buf[1] = {0};
		ssize_t ret = sendto(fd, buf, 0, 0, NULL, 0);
		ASSERT_EQ(0, ret);
		close(fd);
	}
}

TEST(sendto_invalid_fd) {
	uint8_t buf[1] = {0};
	ssize_t ret = sendto(-1, buf, 1, 0, NULL, 0);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(recvfrom);

TEST(recvfrom_zero_length) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd >= 0) {
		uint8_t buf[1] = {0};
		ssize_t ret = recvfrom(fd, buf, 0, 0, NULL, NULL);
		ASSERT_EQ(0, ret);
		close(fd);
	}
}

TEST(recvfrom_invalid_fd) {
	uint8_t buf[1] = {0};
	ssize_t ret = recvfrom(-1, buf, 1, 0, NULL, NULL);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(setsockopt);

TEST(setsockopt_null_optval) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(int));
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(setsockopt_invalid_level) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int val = 1;
		int ret = setsockopt(fd, 99999, SO_REUSEADDR, &val, sizeof(val));
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == ENOPROTOOPT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(setsockopt_zero_optlen) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int val = 1;
		int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, 0);
		ASSERT_TRUE(ret == 0 || ret == -1);
		close(fd);
	}
}

/* ============================================================================ */

TEST_SUITE(getsockopt);

TEST(getsockopt_null_optval) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(getsockopt_invalid_level) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int val = 0;
		socklen_t len = sizeof(val);
		int ret = getsockopt(fd, 99999, SO_REUSEADDR, &val, &len);
		ASSERT_EQ(-1, ret);
		ASSERT_ERRNO(EOPNOTSUPP);
		close(fd);
	}
}

/* ============================================================================ */

TEST_SUITE(sendmsg);

TEST(sendmsg_null_msghdr) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd >= 0) {
		ssize_t ret = sendmsg(fd, NULL, 0);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(sendmsg_invalid_fd) {
	struct msghdr msg = {0};
	ssize_t ret = sendmsg(-1, &msg, 0);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(recvmsg);

TEST(recvmsg_null_msghdr) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd >= 0) {
		ssize_t ret = recvmsg(fd, NULL, 0);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(recvmsg_invalid_fd) {
	struct msghdr msg = {0};
	ssize_t ret = recvmsg(-1, &msg, 0);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(getsockname);

TEST(getsockname_null_addr) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = getsockname(fd, NULL, NULL);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(getsockname_invalid_fd) {
	struct sockaddr sa;
	socklen_t len = sizeof(sa);
	int ret = getsockname(-1, &sa, &len);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOTSOCK || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(getpeername);

TEST(getpeername_null_addr) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = getpeername(fd, NULL, NULL);
		ASSERT_EQ(-1, ret);
		/* ENOTCONN = unconnected socket (checked before NULL validation) */
		ASSERT_TRUE(errno == ENOTCONN || errno == EFAULT || errno == ENOSYS);
		close(fd);
	}
}

TEST(getpeername_invalid_fd) {
	struct sockaddr sa;
	socklen_t len = sizeof(sa);
	int ret = getpeername(-1, &sa, &len);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOTSOCK || errno == ENOSYS);
}

/* ============================================================================ */

TEST_SUITE(shutdown);

TEST(shutdown_invalid_fd) {
	int ret = shutdown(99999, SHUT_RD);
	ASSERT_EQ(-1, ret);
	ASSERT_TRUE(errno == EBADF || errno == ENOTSOCK || errno == ENOSYS);
}

TEST(shutdown_invalid_how) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd >= 0) {
		int ret = shutdown(fd, 999);
		ASSERT_EQ(-1, ret);
		ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
		close(fd);
	}
}

TEST(shutdown_constants_valid) {
	ASSERT_EQ(0, SHUT_RD);
	ASSERT_EQ(1, SHUT_WR);
	ASSERT_EQ(2, SHUT_RDWR);
}

/* ============================================================================ */

TEST_SUITE(socketpair);

TEST(socketpair_null_array) {
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, NULL);
	ASSERT_EQ(-1, ret);
	ASSERT_ERRNO(EFAULT);
}

TEST(socketpair_unsupported_family) {
	int sv[2];
	int ret = socketpair(999, SOCK_STREAM, 0, sv);
	ASSERT_EQ(-1, ret);
	ASSERT_ERRNO(EAFNOSUPPORT);
}

TEST(socketpair_unix_stream) {
	int sv[2];
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
	ASSERT_EQ(0, ret);
	close(sv[0]);
	close(sv[1]);
}

TEST(socketpair_dgram) {
	int sv[2];
	int ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, sv);
	ASSERT_EQ(0, ret);
	close(sv[0]);
	close(sv[1]);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "sys/socket.h requires POSIX\n")
