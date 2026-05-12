/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/serve.h>
#include <transit/conn.h>
#include <transit/http.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

TEST_TYPE(unit);
TEST_UNIT(transit/serve.h);

/* ============================================================================ */
/* Mock Transport (Memory-Backed Connection)                                    */
/* ============================================================================ */

typedef struct {
	conn_t base;
	const uint8_t *read_buf;
	size_t read_len;
	size_t read_pos;
	uint8_t *write_buf;
	size_t write_cap;
	size_t write_len;
	bool closed;
	int read_count;  /* Track how many times read() was called */
} mock_conn_t;

static ssize_t mock_read(void *ctx, void *buf, size_t len) {
	mock_conn_t *mc = (mock_conn_t *)ctx;
	if (!mc || mc->closed || !mc->read_buf) { errno = EBADF; return -1; }

	size_t available = mc->read_len - mc->read_pos;
	if (available == 0) return 0;

	/* Return max 2048 bytes per read (forces multiple reads for keep-alive) */
	size_t to_copy = (len < available) ? len : available;
	if (to_copy > 2048) to_copy = 2048;

	memcpy(buf, mc->read_buf + mc->read_pos, to_copy);
	mc->read_pos += to_copy;
	return (ssize_t)to_copy;
}

static ssize_t mock_write(void *ctx, const void *buf, size_t len) {
	mock_conn_t *mc = (mock_conn_t *)ctx;
	if (!mc || mc->closed) { errno = EBADF; return -1; }

	size_t available = mc->write_cap - mc->write_len;
	size_t to_copy = (len < available) ? len : available;
	memcpy(mc->write_buf + mc->write_len, buf, to_copy);
	mc->write_len += to_copy;
	return (ssize_t)to_copy;
}

static int mock_close(void *ctx) {
	mock_conn_t *mc = (mock_conn_t *)ctx;
	if (!mc) { errno = EBADF; return -1; }
	mc->closed = true;
	return 0;
}

static int mock_pending(void *ctx) {
	mock_conn_t *mc = (mock_conn_t *)ctx;
	if (!mc) { errno = EBADF; return -1; }
	return (mc->read_pos < mc->read_len) ? 1 : 0;
}

static const conn_ops_t mock_ops = {
	.read = mock_read,
	.write = mock_write,
	.recvfrom = NULL,
	.sendto = NULL,
	.close = mock_close,
	.pending = mock_pending,
	.set_timeout = NULL,
	.set_option = NULL
};

static mock_conn_t *mock_conn_create(const uint8_t *input, size_t input_len, size_t write_cap) {
	mock_conn_t *mc = (mock_conn_t *)calloc(1, sizeof(mock_conn_t));
	if (!mc) return NULL;

	mc->read_buf = input;
	mc->read_len = input_len;
	mc->read_pos = 0;
	mc->write_buf = (uint8_t *)malloc(write_cap);
	mc->write_cap = write_cap;
	mc->write_len = 0;
	mc->closed = false;
	mc->read_count = 0;

	mc->base.domain = AF_INET;
	mc->base.socktype = SOCK_STREAM;
	mc->base.protocol = IPPROTO_TCP;
	mc->base.fd = -1;
	mc->base.ctx = mc;
	mc->base.ops = &mock_ops;
	mc->base.owns_fd = false;
	mc->base.type = CONN_STREAM;

	return mc;
}

static void mock_conn_free(mock_conn_t *mc) {
	if (!mc) return;
	free(mc->write_buf);
	free(mc);
}

static int mock_verify_contains(mock_conn_t *mc, const char *expected) {
	if (!mc || !mc->write_buf) return 0;
	char *found = strstr((char *)mc->write_buf, expected);
	return (found != NULL) ? 1 : 0;
}

/* ============================================================================ */
/* File-Scope Handlers                                                          */
/* ============================================================================ */

static void test_ping_handler(serve_conn_t *sc) {
	if (strcmp(sc->req.path, "/ping") == 0) {
	    sc->res.code = HTTP_OK;
	    http_res_body(&sc->res, (uint8_t*)"pong", 4);
	} else {
	    sc->res.code = HTTP_NOT_FOUND;
	}
}

static void test_upgrade_handler(serve_conn_t *sc) {
	if (sc->req.upgrade) {
	    sc->res.code = HTTP_SWITCH;
	    http_res_header(&sc->res, "Upgrade", "websocket");
	    http_res_header(&sc->res, "Connection", "Upgrade");
	} else {
	    sc->res.code = HTTP_OK;
	}
}

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_serve_config_defaults) {
	serve_config_t cfg = {0};
	ASSERT_FALSE(cfg.keep_alive);
	ASSERT_EQ(cfg.timeout_ms, 0);
	ASSERT_EQ(cfg.max_requests, 0);
	ASSERT_EQ(cfg.max_header_size, 0);
	ASSERT_EQ(cfg.max_body_size, 0);
}

/* ============================================================================ */
TEST_SUITE(structures);

TEST(struct_serve_conn_size) {
	ASSERT_SIZE_MAX(serve_conn_t, 4096);
}

TEST(struct_serve_conn_fields) {
	serve_conn_t sc;
	memset(&sc, 0, sizeof(sc));
	ASSERT_NULL(sc.conn);
	ASSERT_NULL(sc.buf);
	ASSERT_FALSE(sc.active);
	ASSERT_EQ(sc.request_count, 0);
}

/* ============================================================================ */
TEST_SUITE(serve_loop);

TEST(serve_loop_null_listen) {
	serve_loop(NULL, NULL);
	ASSERT_TRUE(1);
}

TEST(serve_loop_config_null_listen) {
	serve_config_t cfg = {0};
	serve_loop_config(NULL, NULL, cfg);
	ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_SUITE(serve_accept);

TEST(serve_accept_null_listen) {
	serve_conn_t *sc = serve_accept(NULL, NULL, NULL, 0);
	ASSERT_NULL(sc);
}

TEST(serve_accept_null_buf) {
	conn_t *listen_c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT_NOT_NULL(listen_c);
	conn_bind(listen_c, "127.0.0.1", "0");
	conn_listen(listen_c, 1);
	uint8_t buf[2048];
	serve_conn_t *sc = serve_accept(listen_c, NULL, buf, sizeof(buf));
	if (sc != NULL) serve_free(sc);
	conn_close(listen_c);
}

/* ============================================================================ */
TEST_SUITE(serve_step);

TEST(serve_step_null_conn) {
	int r = serve_step(NULL);
	ASSERT_EQ(r, -1);
}

TEST(serve_step_inactive_conn) {
	serve_conn_t sc = {0};
	sc.active = false;
	int r = serve_step(&sc);
	ASSERT_TRUE(r == -1 || r == 0);
}

/* ============================================================================ */
TEST_SUITE(serve_free);

TEST(serve_free_null) {
	serve_free(NULL);
	ASSERT_TRUE(1);
}

TEST(serve_free_active_conn) {
	serve_conn_t sc = {0};
	sc.active = true;
	sc.buf = malloc(2048);
	serve_free(&sc);
	ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_SUITE(serve_upgrade);

TEST(serve_upgrade_null_conn) {
	serve_upgrade(NULL, NULL, NULL);
	ASSERT_TRUE(1);
}

TEST(serve_upgrade_preserves_buffer) {
	const char *request =
	    "GET /ws HTTP/1.1\r\n"
	    "Host: test\r\n"
	    "Upgrade: websocket\r\n"
	    "Connection: Upgrade\r\n"
	    "\r\n"
	    "EXTRA_DATA";

	mock_conn_t *mc = mock_conn_create((uint8_t*)request, strlen(request), 4096);
	ASSERT_NOT_NULL(mc);

	uint8_t buf[4096];
	serve_conn_t sc = {0};
	sc.conn = &mc->base;
	sc.buf = buf;
	sc.buf_size = sizeof(buf);
	sc.buf_used = 0;  /* Start empty, let mock provide data */
	sc.active = true;
	sc.handler = test_upgrade_handler;
	sc.config.keep_alive = true;
	sc.config.max_requests = 100;

	http_init(&sc.req, buf, sizeof(buf));

	int r = serve_step(&sc);

	ASSERT_EQ(sc.res.code, HTTP_SWITCH);
	ASSERT_TRUE(sc.buf_used > 0);  /* Extra data preserved */

	mock_conn_free(mc);
}

/* ============================================================================ */
TEST_SUITE(serve_config);

TEST(serve_set_config_null) {
	serve_set_config(NULL, (serve_config_t){0});
	ASSERT_TRUE(1);
}

TEST(serve_set_config_keep_alive) {
	serve_conn_t sc = {0};
	sc.config.keep_alive = true;
	serve_config_t new_cfg = {.keep_alive = false};
	serve_set_config(&sc, new_cfg);
	ASSERT_FALSE(sc.config.keep_alive);
}

/* ============================================================================ */
TEST_SUITE(serve_close);

TEST(serve_close_null) {
	serve_close(NULL);
	ASSERT_TRUE(1);
}

TEST(serve_close_sends_response) {
	const char *request = "GET /ping HTTP/1.1\r\nHost: test\r\n\r\n";

	mock_conn_t *mc = mock_conn_create((uint8_t*)request, strlen(request), 4096);
	ASSERT_NOT_NULL(mc);

	uint8_t buf[4096];
	serve_conn_t sc = {0};
	sc.conn = &mc->base;
	sc.buf = buf;
	sc.buf_size = sizeof(buf);
	sc.buf_used = 0;  /* Start empty, let mock provide data */
	sc.active = true;
	sc.handler = test_ping_handler;
	sc.config.keep_alive = true;
	sc.config.max_requests = 100;

	http_init(&sc.req, buf, sizeof(buf));

	int r = serve_step(&sc);

	ASSERT_EQ(r, 0);
	ASSERT_EQ(sc.res.code, HTTP_OK);
	ASSERT_TRUE(mock_verify_contains(mc, "HTTP/1.1 200 OK"));
	ASSERT_TRUE(mock_verify_contains(mc, "pong"));

	serve_close(&sc);
	ASSERT_FALSE(sc.active);
	ASSERT_TRUE(mc->closed);

	mock_conn_free(mc);
}

/* ============================================================================ */
TEST_SUITE(handler_signature);

TEST(handler_receives_serve_conn) {
	serve_handler_t h = test_ping_handler;
	ASSERT_NOT_NULL(h);
}

/* ============================================================================ */
TEST_SUITE(integration_simple);

TEST(serve_accept_then_step) {
	const char *request = "GET /ping HTTP/1.1\r\nHost: test\r\n\r\n";

	mock_conn_t *mc = mock_conn_create((uint8_t*)request, strlen(request), 4096);
	ASSERT_NOT_NULL(mc);

	uint8_t buf[4096];
	serve_conn_t sc = {0};
	sc.conn = &mc->base;
	sc.buf = buf;
	sc.buf_size = sizeof(buf);
	sc.buf_used = 0;  /* Start empty, let mock provide data */
	sc.active = true;
	sc.handler = test_ping_handler;
	sc.config.keep_alive = true;
	sc.config.max_requests = 100;

	http_init(&sc.req, buf, sizeof(buf));

	int r = serve_step(&sc);

	ASSERT_EQ(r, 0);
	ASSERT_EQ(sc.res.code, HTTP_OK);
	ASSERT_TRUE(mock_verify_contains(mc, "pong"));

	mock_conn_free(mc);
}

TEST(serve_keep_alive_multiple_requests) {
	const char *requests =
	    "GET /ping HTTP/1.1\r\nHost: test\r\n\r\n"
	    "GET /ping HTTP/1.1\r\nHost: test\r\n\r\n";

	mock_conn_t *mc = mock_conn_create((uint8_t*)requests, strlen(requests), 8192);
	ASSERT_NOT_NULL(mc);

	uint8_t buf[8192];
	serve_conn_t sc = {0};
	sc.conn = &mc->base;
	sc.buf = buf;
	sc.buf_size = sizeof(buf);
	sc.buf_used = 0;
	sc.active = true;
	sc.handler = test_ping_handler;
	sc.config.keep_alive = true;
	sc.config.max_requests = 100;

	http_init(&sc.req, buf, sizeof(buf));

	/* First request */
	int r1 = serve_step(&sc);
	ASSERT_EQ(r1, 0);
	ASSERT_EQ(sc.request_count, 1);

	/* Reset parser for second request (keep-alive) */
	http_reset(&sc.req);

	/* Second request - mock still has data */
	int r2 = serve_step(&sc);
	ASSERT_EQ(r2, 0);
	ASSERT_EQ(sc.request_count, 2);

	ASSERT_TRUE(mock_verify_contains(mc, "HTTP/1.1 200 OK"));

	mock_conn_free(mc);
}

/* ============================================================================ */

#endif

TEST_MAIN_IF(JACL_HAS_POSIX, "transit/serve.h needs POSIX")
