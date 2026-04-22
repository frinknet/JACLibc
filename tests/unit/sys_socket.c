/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <sys/socket.h>
#include <errno.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(sys/socket.h);

/* ============================================================================
 * CONSTANTS - ADDRESS FAMILIES
 * ============================================================================ */
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
}

TEST(constants_sock_options) {
	//TODO missing options
	ASSERT_EQ(1, SOL_SOCKET);
	ASSERT_EQ(2, SO_REUSEADDR);
	ASSERT_EQ(9, SO_KEEPALIVE);
}

TEST(constants_sock_messages) {
	ASSERT_EQ(0x01, MSG_OOB);
	ASSERT_EQ(0x02, MSG_PEEK);
	ASSERT_EQ(0x40, MSG_DONTWAIT);
}

/* ============================================================================ */

TEST_SUITE(socketaddr);

TEST(sockaddr_size) {
	// Standard sockaddr is 16 bytes (2 family + 14 data)
	ASSERT_EQ(16, sizeof(struct sockaddr));
}

TEST(sockaddr_family) {
	// sa_family must be at offset 0
	struct sockaddr sa;
	ASSERT_OFFSET(struct sockaddr, sa_family, 0);
}

TEST(sockaddr_data) {
	// sa_data must be at offset 2
	struct sockaddr sa;
	ASSERT_OFFSET(struct sockaddr, sa_data, 2);
}

/* ============================================================================
 * FUNCTIONS - BASIC VALIDATION
 * ============================================================================ */
TEST_SUITE(socket);

TEST(socket_invalid_family) {
	// Should return -1 and set errno to EAFNOSUPPORT or EINVAL
	int fd = socket(999, SOCK_STREAM, 0);
	ASSERT_EQ(-1, fd);
	ASSERT_TRUE(errno == EAFNOSUPPORT || errno == EINVAL || errno == ENOSYS);
}

TEST(socket_invalid_type) {
	// Should return -1 and set errno to EINVAL or ENOSYS
	int fd = socket(AF_INET, 999, 0);
	ASSERT_EQ(-1, fd);
	ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
}

TEST(shutdown_constants) {
	ASSERT_EQ(0, SHUT_RD);
	ASSERT_EQ(1, SHUT_WR);
	ASSERT_EQ(2, SHUT_RDWR);
}

/* ============================================================================
 * STRUCTURES - MESSAGE HEADERS (POSIX)
 * ============================================================================ */
TEST_SUITE(message_headers);

TEST(msghdr_exists) {
	struct msghdr msg;
	(void)msg;
	ASSERT_TRUE(1);
}

TEST(msghdr_fields) {
	struct msghdr msg;
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

TEST(cmsghdr_exists) {
	struct cmsghdr cmsg;
	(void)cmsg;
	ASSERT_TRUE(1);
}

TEST(cmsg_macros) {
	// Verify CMSG_SPACE and CMSG_LEN don't crash and return reasonable sizes
	size_t space = CMSG_SPACE(0);
	size_t len = CMSG_LEN(0);

	ASSERT_TRUE(space >= sizeof(struct cmsghdr));
	ASSERT_TRUE(len >= sizeof(struct cmsghdr));
	ASSERT_TRUE(space >= len);
}

TEST_MAIN()

#else

int main(void) {
	printf("sys/socket.h requires POSIX\n");
	return 0;
}

#endif
