/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/route.h>
#include <transit/conn.h>
#include <transit/http.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(transit/route.h);

/* ============================================================================ */
/* File-Scope Handlers                                                          */
/* ============================================================================ */

static int g_handler_called = 0;

static void dummy_handler(serve_conn_t *sc) { 
    (void)sc;
    g_handler_called = 1;
}

static void test_handler_ok(serve_conn_t *sc) {
    if (sc) {
        sc->res.code = HTTP_OK;
        g_handler_called = 1;
    }
}

static void test_handler_not_found(serve_conn_t *sc) {
    if (sc) {
        sc->res.code = HTTP_NOT_FOUND;
        g_handler_called = 1;
    }
}

static int test_mw_continue(serve_conn_t *sc) {
    (void)sc;
    return 0;
}

static int test_mw_shortcircuit(serve_conn_t *sc) {
    if (sc) sc->res.code = HTTP_FORBIDDEN;
    return 1;
}

/* ============================================================================ */
/* Helper: Create minimal serve_conn_t for testing                              */
/* ============================================================================ */

static serve_conn_t make_test_conn(const char *path, http_method_t method) {
    serve_conn_t sc;
    memset(&sc, 0, sizeof(sc));
    
    static uint8_t buf[4096];
    
    sc.conn = NULL;
    sc.buf = buf;
    sc.buf_size = sizeof(buf);
    sc.buf_used = 0;
    sc.active = true;
    sc.req.path = path;
    sc.req.method = method;
    sc.req.hcount = 0;
    sc.res.code = 0;  /* Don't init - let handler set it */
    
    /* Don't call http_res_init() - it may interfere with handler */
    
    return sc;
}

/* ============================================================================ */
TEST_SUITE(path_matching);

TEST(route_path_match_exact) {
    ASSERT_TRUE(route_path_match("/api/users", "/api/users"));
    ASSERT_FALSE(route_path_match("/api/users", "/api/posts"));
    ASSERT_FALSE(route_path_match("/api/users", "/api/users/123"));
}

TEST(route_path_match_prefix) {
    ASSERT_TRUE(route_path_match("/api/", "/api/users"));
    ASSERT_TRUE(route_path_match("/api/", "/api/users/123"));
    ASSERT_TRUE(route_path_match("/api/", "/api/"));
    ASSERT_FALSE(route_path_match("/api/", "/other"));
}

TEST(route_path_match_wildcard) {
    ASSERT_TRUE(route_path_match("/api/users/*", "/api/users/123"));
    ASSERT_TRUE(route_path_match("/api/users/*", "/api/users/abc/def"));
    ASSERT_TRUE(route_path_match("/api/users/*", "/api/users/"));
    ASSERT_FALSE(route_path_match("/api/users/*", "/api/posts"));
    ASSERT_FALSE(route_path_match("/api/users/*", "/api/users"));
}

TEST(route_path_capture) {
    const char *captured;
    ASSERT_TRUE(route_path_capture("/api/users/*", "/api/users/123", &captured));
    ASSERT_STR_EQ("123", captured);
    
    ASSERT_TRUE(route_path_capture("/files/*", "/files/image.png", &captured));
    ASSERT_STR_EQ("image.png", captured);
    
    ASSERT_FALSE(route_path_capture("/api/*", "/other", &captured));
    ASSERT_NULL(captured);
}

/* ============================================================================ */
TEST_SUITE(route_creation);

TEST(route_new) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    route_free(r);
}

TEST(route_use) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    route_t *result = route_use(r, route_middleware_log);
    ASSERT_PTR_EQ(result, r);
    route_free(r);
}

TEST(route_handle) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    g_handler_called = 0;
    route_t *result = route_handle(r, "/test", dummy_handler);
    ASSERT_PTR_EQ(result, r);
    ASSERT_EQ(r->count, 1);  /* Verify entry was added */
    route_free(r);
}

TEST(route_match) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    route_t *result = route_match(r, "/api/", route_middleware_auth);
    ASSERT_PTR_EQ(result, r);
    route_free(r);
}

TEST(route_group) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    route_t *g = route_group(r, "/api");
    ASSERT_NOT_NULL(g);
    ASSERT_PTR_EQ(g->parent, r);
    ASSERT_STR_EQ("/api", g->prefix);
    route_free(g);
    route_free(r);
}

TEST(route_fallback) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    route_t *result = route_fallback(r, dummy_handler);
    ASSERT_PTR_EQ(result, r);
    route_free(r);
}

/* ============================================================================ */
TEST_SUITE(dispatch);

TEST(dispatch_exact_match) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_handle(r, "/ping", test_handler_ok);
    ASSERT_EQ(r->count, 1);  /* Verify route was added */
    
    serve_conn_t sc = make_test_conn("/ping", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);  /* Verify handler ran */
    ASSERT_EQ(sc.res.code, HTTP_OK);
    route_free(r);
}

TEST(dispatch_prefix_match) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_handle(r, "/api/", test_handler_ok);
    
    serve_conn_t sc = make_test_conn("/api/users", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);
    ASSERT_EQ(sc.res.code, HTTP_OK);
    route_free(r);
}

TEST(dispatch_wildcard_match) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_handle(r, "/files/*", test_handler_ok);
    
    serve_conn_t sc = make_test_conn("/files/image.png", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);
    ASSERT_EQ(sc.res.code, HTTP_OK);
    route_free(r);
}

TEST(dispatch_no_match_fallback) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_fallback(r, test_handler_not_found);
    
    serve_conn_t sc = make_test_conn("/unknown", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);
    ASSERT_EQ(sc.res.code, HTTP_NOT_FOUND);
    route_free(r);
}

TEST(dispatch_no_match_no_fallback) {
    route_t *r = route_new();
    
    serve_conn_t sc = make_test_conn("/unknown", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_EQ(sc.res.code, HTTP_NOT_FOUND);
    route_free(r);
}

TEST(dispatch_middleware_chain) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_use(r, test_mw_continue);
    route_handle(r, "/test", test_handler_ok);
    
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);
    ASSERT_EQ(sc.res.code, HTTP_OK);
    route_free(r);
}

TEST(dispatch_middleware_shortcircuit) {
    route_t *r = route_new();
    route_use(r, test_mw_shortcircuit);
    
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_EQ(sc.res.code, HTTP_FORBIDDEN);
    route_free(r);
}

TEST(dispatch_group_prefix) {
    route_t *r = route_new();
    g_handler_called = 0;
    route_t *g = route_group(r, "/api");
    route_handle(g, "/users", test_handler_ok);
    
    serve_conn_t sc = make_test_conn("/api/users", HTTP_GET);
    route_dispatch(r, &sc);
    
    ASSERT_TRUE(g_handler_called);
    ASSERT_EQ(sc.res.code, HTTP_OK);
    route_free(g);
    route_free(r);
}

TEST(dispatch_null_inputs) {
    route_dispatch(NULL, NULL);
    ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_SUITE(built_in_middleware);

TEST(middleware_log) {
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    int r = route_middleware_log(&sc);
    ASSERT_EQ(r, 0);
}

TEST(middleware_cors_normal) {
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    int r = route_middleware_cors(&sc);
    ASSERT_EQ(r, 0);
}

TEST(middleware_cors_preflight) {
    serve_conn_t sc = make_test_conn("/test", HTTP_OPTIONS);
    int r = route_middleware_cors(&sc);
    ASSERT_EQ(r, 1);
    ASSERT_EQ(sc.res.code, HTTP_NO_CONTENT);
}

TEST(middleware_auth_valid) {
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    sc.req.hdrs[0].key = "Authorization";
    sc.req.hdrs[0].val = "Bearer token123";
    sc.req.hcount = 1;
    
    int r = route_middleware_auth(&sc);
    ASSERT_EQ(r, 0);
}

TEST(middleware_auth_missing) {
    serve_conn_t sc = make_test_conn("/test", HTTP_GET);
    int r = route_middleware_auth(&sc);
    ASSERT_EQ(r, 1);
    ASSERT_EQ(sc.res.code, HTTP_UNAUTHORIZED);
}

/* ============================================================================ */
TEST_SUITE(edge_cases);

TEST(route_overflow_protection) {
    route_t *r = route_new();
    ASSERT_NOT_NULL(r);
    
    for (int i = 0; i < ROUTE_MAX_ENTRIES; i++) {
        route_handle(r, "/test", dummy_handler);
    }
    
    int before = r->count;
    route_handle(r, "/overflow", dummy_handler);
    ASSERT_EQ(r->count, before);
    
    route_free(r);
}

TEST(route_null_pattern) {
    route_t *r = route_new();
    route_handle(r, NULL, NULL);
    route_match(r, NULL, NULL);
    route_free(r);
    ASSERT_TRUE(1);
}

TEST(route_null_handler) {
    route_t *r = route_new();
    route_handle(r, "/test", NULL);
    route_free(r);
    ASSERT_TRUE(1);
}

TEST(dispatch_null_sc) {
    route_t *r = route_new();
    route_dispatch(r, NULL);
    route_free(r);
    ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "transit/route.h needs POSIX")

#else

int main(void) {
    printf("transit/route.h requires POSIX\n");
    return 0;
}

#endif
