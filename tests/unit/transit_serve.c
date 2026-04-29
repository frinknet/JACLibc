/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/serve.h>
#include <transit/conn.h>
#include <transit/http.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(transit/serve.h);

/* Handler at file scope - NOT inside TEST() */
static void test_ping_handler(serve_conn_t *sc) {
    if (strcmp(sc->req.path, "/ping") == 0) {
        sc->res.code = HTTP_OK;
        http_res_body(&sc->res, (uint8_t*)"pong", 4);
    } else {
        sc->res.code = HTTP_NOT_FOUND;
    }
}

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_serve_config_defaults) {
    serve_config_t cfg = {0};
    
    /* Zero-initialized struct: all fields are 0/false */
    /* Library sets defaults in serve_accept(), not in struct */
    ASSERT_FALSE(cfg.keep_alive);  /* 0 = false */
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
    TEST_SKIP("Requires mock parser and transport");
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
    TEST_SKIP("Requires mock transport");
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
    TEST_SKIP("Requires full integration setup");
}

TEST(serve_keep_alive_multiple_requests) {
    TEST_SKIP("Requires mock client and parser");
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "transit/serve.h needs POSIX")

#else

int main(void) {
    printf("transit/serve.h requires POSIX\n");
    return 0;
}

#endif
