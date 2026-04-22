/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <sys/un.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys/un.h);

/* ============================================================================
 * CONSTANTS & MACROS
 * ============================================================================ */
TEST_SUITE(sun_len);

TEST(sun_len_empty) {
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	/* Empty path = header size + null terminator */
	ASSERT_EQ(sizeof(addr.sun_family) + 1, SUN_LEN(&addr));
}

TEST(sun_len_standard) {
	struct sockaddr_un addr;
	sockaddr_un_init(&addr, "/tmp/test.sock");

	size_t expected = offsetof(struct sockaddr_un, sun_path) + strlen("/tmp/test.sock") + 1;
	ASSERT_EQ(expected, SUN_LEN(&addr));
}

TEST(sun_len_exact_boundary) {
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	memset(addr.sun_path, 'A', UNIX_PATH_MAX - 1);
	addr.sun_path[UNIX_PATH_MAX - 1] = '\0';

	size_t expected = offsetof(struct sockaddr_un, sun_path) + UNIX_PATH_MAX;
	ASSERT_EQ(expected, SUN_LEN(&addr));
}

TEST(sun_len_abstract_socket) {
	/* Linux abstract namespace: sun_path[0] == '\0' */
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	addr.sun_path[0] = '\0';

	/* Macro returns header size + 1 when first char is null */
	ASSERT_EQ(offsetof(struct sockaddr_un, sun_path) + 1, SUN_LEN(&addr));
}
/* ============================================================================ */

TEST_SUITE(sockaddr_un);

TEST(sockaddr_un_init_valid) {
	struct sockaddr_un addr;
	sockaddr_un_init(&addr, "/var/run/mydaemon.sock");

	ASSERT_EQ(AF_UNIX, addr.sun_family);
	ASSERT_STR_EQ("/var/run/mydaemon.sock", addr.sun_path);
	ASSERT_EQ('\0', addr.sun_path[strlen("/var/run/mydaemon.sock")]);
}

TEST(sockaddr_un_init_truncate) {
	struct sockaddr_un addr;
	char long_path[200];
	memset(long_path, 'A', sizeof(long_path) - 1);
	long_path[sizeof(long_path) - 1] = '\0';

	sockaddr_un_init(&addr, long_path);

	ASSERT_EQ(AF_UNIX, addr.sun_family);
	ASSERT_EQ(UNIX_PATH_MAX - 1, strlen(addr.sun_path));
	ASSERT_EQ('\0', addr.sun_path[UNIX_PATH_MAX - 1]);
}

TEST(sockaddr_un_init_nulls) {
	sockaddr_un_init(NULL, "/tmp/test");
	sockaddr_un_init((struct sockaddr_un *)0x0, NULL);
	ASSERT_TRUE(1);
}

TEST(sockaddr_un_init_embedded_null) {
	struct sockaddr_un addr;
	char path[] = "/tmp/test\0hidden";
	sockaddr_un_init(&addr, path);

	ASSERT_STR_EQ("/tmp/test", addr.sun_path);
	ASSERT_EQ(9, strlen(addr.sun_path));
}

TEST(sockaddr_un_abi_alignment) {
	/* Verify no hidden padding breaks wire format */
	ASSERT_EQ(0, offsetof(struct sockaddr_un, sun_family) % 2);
	ASSERT_EQ(0, offsetof(struct sockaddr_un, sun_path) % 1);
	ASSERT_EQ(sizeof(struct sockaddr_un), 2 + UNIX_PATH_MAX);
}

TEST_MAIN()

#else

int main(void) {
	printf("sys/un.h requires POSIX\n");
	return 0;
}

#endif
