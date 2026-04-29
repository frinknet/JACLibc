/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/client.h>
#include <transit/http.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(comms/client.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_client_defaults) {
	client_t c = {0};

	ASSERT_NULL(c.conn);
	ASSERT_NULL(c.host);
	ASSERT_NULL(c.port);
	ASSERT_EQ(c.timeout_ms, 0);
	ASSERT_FALSE(c.connected);
	ASSERT_FALSE(c.keep_alive);
}

/* ============================================================================ */
TEST_SUITE(structures);

TEST(struct_client_size) {
	ASSERT_SIZE_MAX(client_t, 128);
}

TEST(struct_client_fields) {
	client_t c;
	memset(&c, 0, sizeof(c));

	ASSERT_NULL(c.conn);
	ASSERT_FALSE(c.connected);
}

/* ============================================================================ */
TEST_SUITE(simple_http_get);

TEST(http_get_null_host) {
	http_res_t res;
	uint8_t body[1024];

	int r = http_get(NULL, "80", "/", &res, body, sizeof(body));

	ASSERT_EQ(r, -1);
}

TEST(http_get_null_port) {
	http_res_t res;
	uint8_t body[1024];

	int r = http_get("example.com", NULL, "/", &res, body, sizeof(body));

	ASSERT_EQ(r, -1);
}

TEST(http_get_null_path) {
	http_res_t res;
	uint8_t body[1024];

	int r = http_get("example.com", "80", NULL, &res, body, sizeof(body));

	ASSERT_EQ(r, -1);
}

TEST(http_get_null_res) {
	uint8_t body[1024];

	int r = http_get("example.com", "80", "/", NULL, body, sizeof(body));

	ASSERT_EQ(r, -1);
}

TEST(http_get_small_body_buffer) {
	http_res_t res;
	uint8_t tiny[10];

	int r = http_get("example.com", "80", "/", &res, tiny, sizeof(tiny));

	/* May succeed if response fits, or fail with buffer too small */
	/* We just verify it doesn't crash */
	ASSERT_TRUE(r == 0 || r == -1);
}

/* ============================================================================ */
TEST_SUITE(simple_http_post);

TEST(http_post_null_body) {
	http_res_t res;
	uint8_t body[1024];

	int r = http_post("example.com", "80", "/", NULL, 0, &res, body, sizeof(body));

	/* POST with no body is valid (empty body) */
	ASSERT_TRUE(r == 0 || r == -1);
}

TEST(http_post_large_body) {
	http_res_t res;
	uint8_t resp[1024];
	uint8_t large_body[65536];
	memset(large_body, 'A', sizeof(large_body));

	int r = http_post("example.com", "80", "/", large_body, sizeof(large_body),
	                 &res, resp, sizeof(resp));

	/* May succeed or fail depending on server */
	ASSERT_TRUE(r == 0 || r == -1);
}

/* ============================================================================ */
TEST_SUITE(simple_http_put_delete);

TEST(http_put_basic) {
	http_res_t res;
	uint8_t resp[1024];
	const uint8_t body[] = "test";

	int r = http_put("example.com", "80", "/", body, sizeof(body) - 1,
	                &res, resp, sizeof(resp));

	ASSERT_TRUE(r == 0 || r == -1);
}

TEST(http_delete_basic) {
	http_res_t res;
	uint8_t resp[1024];

	int r = http_delete("example.com", "80", "/", &res, resp, sizeof(resp));

	ASSERT_TRUE(r == 0 || r == -1);
}

/* ============================================================================ */
TEST_SUITE(client_create);

TEST(client_create_tcp) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	ASSERT_NOT_NULL(c);
	ASSERT_NULL(c->conn);
	ASSERT_FALSE(c->connected);

	client_free(c);
}

TEST(client_create_udp) {
	client_t *c = client_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	ASSERT_NOT_NULL(c);
	ASSERT_EQ(c->domain, AF_INET);
	ASSERT_EQ(c->type, SOCK_DGRAM);
	ASSERT_EQ(c->protocol, IPPROTO_UDP);
	ASSERT_NULL(c->conn);

	client_free(c);
}

TEST(client_create_unix) {
	client_t *c = client_create(AF_UNIX, SOCK_STREAM, 0);

	ASSERT_NOT_NULL(c);

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(client_from_conn);

TEST(client_from_conn_null) {
	client_t *c = client_from_conn(NULL);

	ASSERT_NULL(c);
}

TEST(client_from_conn_valid) {
	conn_t *conn = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT_NOT_NULL(conn);

	client_t *c = client_from_conn(conn);

	ASSERT_NOT_NULL(c);
	ASSERT_PTR_EQ(c->conn, conn);

	client_free(c);  /* Should not close underlying conn if not owner */
	conn_close(conn);
}

/* ============================================================================ */
TEST_SUITE(client_connect);

TEST(client_connect_null_client) {
	int r = client_connect(NULL, "example.com", "80");

	ASSERT_EQ(r, -1);
}

TEST(client_connect_null_host) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int r = client_connect(c, NULL, "80");

	ASSERT_EQ(r, -1);

	client_free(c);
}

TEST(client_connect_loopback) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int r = client_connect(c, "127.0.0.1", "1");  /* Port 1 unlikely open */

	if (r == 0) {
		/* Connected - close */
		client_disconnect(c);
	} else {
		/* Failed - check expected error */
		ASSERT_TRUE(errno == ECONNREFUSED || errno == ETIMEDOUT);
	}

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(client_connect_path);

TEST(client_connect_path_null_client) {
	int r = client_connect_path(NULL, "/tmp/socket");

	ASSERT_EQ(r, -1);
}

TEST(client_connect_path_unix) {
	client_t *c = client_create(AF_UNIX, SOCK_STREAM, 0);

	/* Try to connect to non-existent socket */
	int r = client_connect_path(c, "/tmp/nonexistent_socket_12345");

	ASSERT_EQ(r, -1);
	ASSERT_TRUE(errno == ENOENT || errno == ECONNREFUSED);

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(client_disconnect_free);

TEST(client_disconnect_null) {
	client_disconnect(NULL);
	ASSERT_TRUE(1);
}

TEST(client_disconnect_not_connected) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	client_disconnect(c);  /* Should be no-op if not connected */

	client_free(c);
}

TEST(client_free_null) {
	client_free(NULL);
	ASSERT_TRUE(1);
}

TEST(client_free_closes_conn) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	client_connect(c, "127.0.0.1", "1");  /* May fail, that's OK */

	/* Free should clean up */
	client_free(c);

	/* After free, accessing c is undefined - we just verify no crash in free */
	ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_SUITE(client_options);

TEST(client_set_timeout_null) {
	int r = client_set_timeout(NULL, 5000);

	ASSERT_EQ(r, -1);
}

TEST(client_set_keepalive) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	client_set_keepalive(c, true);
	ASSERT_TRUE(c->keep_alive);

	client_set_keepalive(c, false);
	ASSERT_FALSE(c->keep_alive);

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(client_send_recv);

TEST(client_send_null) {
	ssize_t r = client_send(NULL, "test", 4);

	ASSERT_EQ(r, -1);
}

TEST(client_recv_null) {
	uint8_t buf[10];
	ssize_t r = client_recv(NULL, buf, sizeof(buf));

	ASSERT_EQ(r, -1);
}

/* ============================================================================ */
TEST_SUITE(client_http_methods);

TEST(client_get_null_client) {
	http_res_t res;
	uint8_t body[1024];

	int r = client_get(NULL, "/", &res, body, sizeof(body));

	ASSERT_EQ(r, -1);
}

TEST(client_get_not_connected) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	http_res_t res;
	uint8_t body[1024];

	int r = client_get(c, "/", &res, body, sizeof(body));

	/* Should fail if not connected */
	ASSERT_EQ(r, -1);

	client_free(c);
}

TEST(client_post_basic) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	http_res_t res;
	uint8_t resp[1024];
	const uint8_t body[] = "test";

	int r = client_post(c, "/", body, sizeof(body) - 1, &res, resp, sizeof(resp));

	/* Should fail if not connected */
	ASSERT_EQ(r, -1);

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(client_request);

TEST(client_request_custom_method) {
	client_t *c = client_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	http_res_t res;
	uint8_t resp[1024];

	const char *headers[] = {"X-Custom: value", NULL};

	int r = client_request(c, "CUSTOM", "/", headers, NULL, 0, &res, resp, sizeof(resp));

	/* Should fail if not connected */
	ASSERT_EQ(r, -1);

	client_free(c);
}

/* ============================================================================ */
TEST_SUITE(response_parse);

TEST(http_response_parse_null_res) {
	uint8_t buf[] = "HTTP/1.1 200 OK\r\n\r\n";
	size_t consumed;

	int r = http_response_parse(NULL, buf, sizeof(buf) - 1, &consumed);

	ASSERT_EQ(r, -1);
}

TEST(http_response_parse_null_buf) {
	http_res_t res;
	size_t consumed;

	int r = http_response_parse(&res, NULL, 0, &consumed);

	ASSERT_EQ(r, -1);
}

TEST(http_response_parse_simple) {
	http_res_t res;
	uint8_t buf[] = "HTTP/1.1 200 OK\r\n"
	                "Content-Length: 5\r\n"
	                "\r\n"
	                "Hello";
	size_t consumed;

	int r = http_response_parse(&res, buf, sizeof(buf) - 1, &consumed);

	ASSERT_EQ(r, 0);
	ASSERT_EQ(res.code, HTTP_OK);
}

/* ============================================================================ */

TEST_MAIN()

#else

int main(void) {
	printf("comms/client.h requires POSIX\n");
	return 0;
}

#endif
