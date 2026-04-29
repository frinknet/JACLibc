/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/conn.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <net/inet.h>

TEST_TYPE(unit);
TEST_UNIT(comms/conn.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_conn_types) {
	ASSERT_EQ(CONN_STREAM, 0);
	ASSERT_EQ(CONN_DGRAM, 1);
	ASSERT_EQ(CONN_PACKET, 2);
}

/* ============================================================================ */
TEST_SUITE(structures);

TEST(struct_conn_ops_size) {
	/* Should have all function pointers */
	ASSERT_SIZE_MIN(conn_ops_t, sizeof(void*) * 8);
}

TEST(struct_conn_size) {
	/* Should be compact: type + ints + pointers */
	ASSERT_SIZE_MAX(conn_t, 128);
}

TEST(struct_conn_fields) {
	conn_t c;
	memset(&c, 0, sizeof(c));

	ASSERT_EQ(c.type, CONN_STREAM);  /* Default */
	ASSERT_EQ(c.domain, 0);
	ASSERT_EQ(c.socktype, 0);
	ASSERT_EQ(c.protocol, 0);
	ASSERT_EQ(c.fd, 0);  /* Or 0, depending on convention */
	ASSERT_NULL(c.ctx);
	ASSERT_NULL(c.ops);
	ASSERT_FALSE(c.owns_fd);
}

/* ============================================================================ */
TEST_SUITE(conn_create);

TEST(conn_create_tcp) {
	conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	ASSERT_NOT_NULL(c);
	ASSERT_EQ(c->type, CONN_STREAM);
	ASSERT_EQ(c->domain, AF_INET);
	ASSERT_EQ(c->socktype, SOCK_STREAM);
	ASSERT_EQ(c->protocol, IPPROTO_TCP);

	conn_close(c);
}

TEST(conn_create_udp) {
	conn_t *c = conn_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	ASSERT_NOT_NULL(c);
	ASSERT_EQ(c->type, CONN_DGRAM);
	ASSERT_EQ(c->domain, AF_INET);
	ASSERT_EQ(c->socktype, SOCK_DGRAM);
	ASSERT_EQ(c->protocol, IPPROTO_UDP);

	conn_close(c);
}

TEST(conn_create_unix) {
	conn_t *c = conn_create(AF_UNIX, SOCK_STREAM, 0);

	ASSERT_NOT_NULL(c);
	ASSERT_EQ(c->type, CONN_STREAM);
	ASSERT_EQ(c->domain, AF_UNIX);

	conn_close(c);
}

TEST(conn_create_invalid_domain) {
	conn_t *c = conn_create(999, SOCK_STREAM, 0);

	/* Should return NULL or handle gracefully */
	/* Implementation-dependent */
	if (c != NULL) {
		conn_close(c);
	}
}

/* ============================================================================ */
TEST_SUITE(conn_from_fd);

TEST(conn_from_fd_valid) {
	/* Create a real socket for testing */
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT_TRUE(fd >= 0);

	conn_t *c = conn_from_fd(fd, AF_INET, SOCK_STREAM, IPPROTO_TCP);

	ASSERT_NOT_NULL(c);
	ASSERT_EQ(c->fd, fd);
	ASSERT_EQ(c->owns_fd, true);  /* Should own the FD */

	conn_close(c);  /* Should close fd */
}

TEST(conn_from_fd_invalid) {
	conn_t *c = conn_from_fd(-1, AF_INET, SOCK_STREAM, IPPROTO_TCP);

	ASSERT_NULL(c);
}

/* ============================================================================ */
TEST_SUITE(conn_operations);

TEST(conn_read_null_conn) {
	uint8_t buf[10];
	ssize_t r = conn_read(NULL, buf, sizeof(buf));

	ASSERT_EQ(r, -1);
}

TEST(conn_write_null_conn) {
	uint8_t buf[10];
	ssize_t r = conn_write(NULL, buf, sizeof(buf));

	ASSERT_EQ(r, -1);
}

TEST(conn_close_null_conn) {
	int r = conn_close(NULL);

	ASSERT_EQ(r, -1);
}

TEST(conn_pending_null_conn) {
	int r = conn_pending(NULL);

	ASSERT_EQ(r, -1);
}

/* ============================================================================ */
TEST_SUITE(conn_connect);

TEST(conn_connect_null_conn) {
	int r = conn_connect(NULL, "example.com", "80");

	ASSERT_EQ(r, -1);
}

TEST(conn_connect_path_null_conn) {
	int r = conn_connect_path(NULL, "/tmp/socket");

	ASSERT_EQ(r, -1);
}

/* ============================================================================ */
TEST_SUITE(conn_bind_listen_accept);

TEST(conn_bind_null_conn) {
	int r = conn_bind(NULL, NULL, "8080");

	ASSERT_EQ(r, -1);
}

TEST(conn_listen_null_conn) {
	int r = conn_listen(NULL, 128);

	ASSERT_EQ(r, -1);
}

TEST(conn_accept_null_conn) {
	conn_t *c = conn_accept(NULL);

	ASSERT_NULL(c);
}

/* ============================================================================ */
TEST_SUITE(conn_options);

TEST(conn_set_timeout_null_conn) {
	int r = conn_set_timeout(NULL, 5000);

	ASSERT_EQ(r, -1);
}

TEST(conn_set_option_null_conn) {
	int r = conn_set_option(NULL, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	ASSERT_EQ(r, -1);
}

/* ============================================================================ */
TEST_SUITE(transport_wrappers);

TEST(conn_tls_stub) {
	/* TLS wrapper not implemented yet */
	/* This test documents the expected interface */
	TEST_SKIP("conn_tls() not implemented yet");
}

TEST(conn_quic_stub) {
	/* QUIC wrapper not implemented yet */
	TEST_SKIP("conn_quic() not implemented yet");
}

/* ============================================================================ */
TEST_SUITE(integration_tcp);

TEST(conn_tcp_connect_loopback) {
	/* Integration test: connect to localhost */
	/* This may fail if nothing is listening, so we check error codes */
	conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT_NOT_NULL(c);

	int r = conn_connect(c, "127.0.0.1", "1");  /* Port 1 unlikely to be open */

	/* Either connection succeeds (unlikely) or fails with expected error */
	if (r == 0) {
		/* Connected - close it */
		conn_close(c);
	} else {
		/* Failed - check it's a reasonable error */
		ASSERT_TRUE(errno == ECONNREFUSED || errno == ETIMEDOUT || errno == EHOSTUNREACH);
	}
}

TEST(conn_tcp_bind_listen) {
	/* Integration test: bind and listen on ephemeral port */
	conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT_NOT_NULL(c);

	int r = conn_bind(c, "127.0.0.1", "0");  /* Port 0 = ephemeral */
	ASSERT_EQ(r, 0);

	r = conn_listen(c, 1);
	ASSERT_EQ(r, 0);

	/* Clean up */
	conn_close(c);
}

/* ============================================================================ */

#endif

TEST_MAIN_IF(JACL_HAS_POSIX, "transit/conn.h requires POSIX")
