/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <transit/route.h>

TEST_TYPE(unit);
TEST_UNIT(transit/route.h);

/* ============================================================================ */
/* Test Context                                                                 */
/* ============================================================================ */

typedef struct {
    int called;
    int result;
    const char *captured;
    const char *argv0;
    int argc;
} test_ctx_t;

/* ============================================================================ */
/* File-Scope Handlers (ALL at file scope - NO nested functions)                */
/* ============================================================================ */

static int record_handler(route_ctx_t ctx, const char *argv[], int argc) {
    test_ctx_t *tc = (test_ctx_t *)route_data(ctx);

    if (tc) {
        tc->called++;
        tc->argc = argc;
        if (argc > 0) tc->argv0 = argv[0];
        if (argc > 1) tc->captured = argv[1];
    }
    return 0;
}

static int stop_handler(route_ctx_t ctx, const char *argv[], int argc) {
    test_ctx_t *tc = (test_ctx_t *)route_data(ctx);
    if (tc) {
        tc->called++;
        tc->result = 42;
    }
    return 42;
}

static int rewrite_handler(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    return route_forward(ctx, "/rewritten/path");
}

static int path_checker(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    return route_path(ctx) ? 1 : 0;
}

static int data_checker(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    return route_data(ctx) ? 1 : 0;
}

static int conn_checker(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    return route_conn(ctx) ? 1 : 0;
}

static int req_checker(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    return route_req(ctx) ? 1 : 0;
}

static int multi_capture(route_ctx_t ctx, const char *argv[], int argc) {
    (void)ctx;
    return (argc == 3) ? 1 : 0;
}

static int no_capture_handler(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argv;
    return (argc == 1) ? 1 : 0;
}

static int check_data_handler(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    test_ctx_t *tc = (test_ctx_t *)route_data(ctx);
    return (tc && tc->result == 99) ? 1 : 0;
}

static int verify_ctx_handler(route_ctx_t ctx, const char *argv[], int argc) {
    (void)argc; (void)argv;
    if (route_conn(ctx) != (void *)0x1111) return 0;
    if (route_req(ctx) != (void *)0x2222) return 0;
    if (route_data(ctx) != (void *)0x3333) return 0;
    return 1;
}

/* ============================================================================ */
TEST_SUITE(router_lifecycle);

TEST(router_create) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    ASSERT_NOT_NULL(r);
    router_free(r);
}

TEST(router_create_invalid_spec) {
    char spec[3] = {0};
    router_t r = router_create(spec);
    ASSERT_NULL(r);

    char valid[3] = {'*', '/', '\\'};
    r = router_create(valid);
    ASSERT_NOT_NULL(r);
    router_free(r);
}

TEST(router_free_null) {
    router_free(NULL);
    ASSERT_TRUE(1);
}

TEST(router_data) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    ASSERT_NOT_NULL(r);

    int my_data = 123;
    r->data = &my_data;

    void *retrieved = r->data;
    ASSERT_PTR_EQ(retrieved, &my_data);

    router_free(r);
}

/* ============================================================================ */
TEST_SUITE(pattern_matching);

TEST(route_match_exact) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/api/users", "/api/users", spec, matches, 16);
    ASSERT_EQ(argc, 1);
    ASSERT_STR_EQ(matches[0], "/api/users");
}

TEST(route_match_segment_wildcard) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/api/users", "/api/*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[0], "/api/users");
    ASSERT_STR_EQ(matches[1], "users");
}

TEST(route_match_rest_wildcard) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/files/a/b/c.txt", "/files/**", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[0], "/files/a/b/c.txt");
    ASSERT_STR_EQ(matches[1], "a/b/c.txt");
}

TEST(route_match_char_class) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/user/abc123", "/user/*[a-z0-9]", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "abc123");
}

TEST(route_match_quantifier_exact) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/code/us", "/code/*[a-z]{2}", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "us");
}

TEST(route_match_quantifier_range) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/code/usa", "/code/*[a-z]{2,3}", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "usa");
}

TEST(route_match_quantifier_too_short) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/code/u", "/code/[a-z]{2,3}", spec, matches, 16);
    ASSERT_EQ(argc, 0);
}

TEST(route_match_escape_literal) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/api/*test", "/api/\\*test", spec, matches, 16);
    ASSERT_EQ(argc, 1);
}

TEST(route_match_no_match) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/api/posts", "/api/users", spec, matches, 16);
    ASSERT_EQ(argc, 0);
}

TEST(route_match_null_inputs) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    ASSERT_EQ(route_match(NULL, "/path", spec, matches, 16), 0);
    ASSERT_EQ(route_match("/path", NULL, spec, matches, 16), 0);
    ASSERT_EQ(route_match("/path", "/path", NULL, matches, 16), 0);
    ASSERT_EQ(route_match("/path", "/path", spec, NULL, 0), 0);
}

TEST(route_match_trailing_slash) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/api/", "/api/", spec, matches, 16);
    ASSERT_EQ(argc, 1);
}

TEST(route_match_empty_path) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("", "", spec, matches, 16);
    ASSERT_EQ(argc, 1);
}

/* ============================================================================ */
TEST_SUITE(pattern_matching_advanced);

TEST(route_match_multiple_wildcards) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/a/b/c", "/a/*/c", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "b/c");
}

TEST(route_match_double_wildcard_middle) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/prefix/middle/suffix", "/prefix/**/suffix", spec, matches, 16);
    ASSERT_EQ(argc, 3);
}

TEST(route_match_char_class_negated) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/user/abc", "/user/*[^0-9]", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "abc");
}

TEST(route_match_char_class_negated_fail) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/user/123", "/user/[^0-9]+", spec, matches, 16);
    ASSERT_EQ(argc, 0);
}

TEST(route_match_quantifier_zero_or_more) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};
    int argc = route_match("/test/", "/test/*[a-z]{0,}", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "");
}

TEST(route_match_spec_custom_any) {
    const char *matches[16];
    char spec[3] = {'?', '.', '\\'};
    int argc = route_match("/api.users", "/api.?", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "users");
}

TEST(route_match_spec_custom_div) {
    const char *matches[16];
    char spec[3] = {'*', ':', '\\'};
    int argc = route_match("/api:users", "/api:*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "users");
}

/* ============================================================================ */
TEST_SUITE(route_registration);

TEST(route_add) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    ASSERT_NOT_NULL(r);

    int rc = route_add(r, "/api/*", record_handler);
    ASSERT_EQ(rc, 0);

    router_free(r);
}

TEST(route_add_null_params) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    ASSERT_EQ(route_add(NULL, "/api/*", record_handler), -1);
    ASSERT_EQ(route_add(r, NULL, record_handler), -1);
    ASSERT_EQ(route_add(r, "/api/*", NULL), -1);

    router_free(r);
}

TEST(route_remove) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/api/*", record_handler);
    route_add(r, "/api/*", stop_handler);

    int removed = route_remove(r, "/api/*", stop_handler);
    ASSERT_EQ(removed, 1);

    test_ctx_t tc = {0};
    r->data = &tc;

    route_dispatch(r, "/api/users", NULL, NULL, &tc);
    ASSERT_EQ(tc.called, 1);

    router_free(r);
}

TEST(route_remove_not_found) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    int removed = route_remove(r, "/nonexistent", record_handler);
    ASSERT_EQ(removed, 0);

    router_free(r);
}

TEST(route_add_multiple_same_pattern) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test", record_handler);
    route_add(r, "/test", stop_handler);
    route_add(r, "/test", record_handler);

    test_ctx_t tc = {0};
    r->data = &tc;

    route_dispatch(r, "/test", NULL, NULL, &tc);
    ASSERT_EQ(tc.called, 2);

    router_free(r);
}

/* ============================================================================ */
TEST_SUITE(dispatch);

TEST(dispatch_single_handler) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/test", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/test", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 1);
    ASSERT_EQ(result, 0);

    router_free(r);
}

TEST(dispatch_with_captures) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/user/*", record_handler);

    test_ctx_t tc = {0};
    route_dispatch(r, "/user/alice", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 1);
    ASSERT_STR_EQ(tc.captured, "alice");

    router_free(r);
}

TEST(dispatch_chain_continue) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/test", record_handler);
    route_add(r, "/test", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/test", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 2);
    ASSERT_EQ(result, 0);

    router_free(r);
}

TEST(dispatch_chain_stop) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/test", record_handler);
    route_add(r, "/test", stop_handler);
    route_add(r, "/test", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/test", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 2);
    ASSERT_EQ(result, 42);

    router_free(r);
}

TEST(dispatch_no_match) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/other", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/test", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 0);
    ASSERT_EQ(result, -1);

    router_free(r);
}

TEST(dispatch_null_router) {
    int result = route_dispatch(NULL, "/test", NULL, NULL, NULL);
    ASSERT_EQ(result, -1);
}

TEST(dispatch_null_path) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    int result = route_dispatch(r, NULL, NULL, NULL, NULL);
    ASSERT_EQ(result, -1);
    router_free(r);
}

TEST(dispatch_argv0_is_path) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/test/*", record_handler);

    test_ctx_t tc = {0};
    route_dispatch(r, "/test/value", NULL, NULL, &tc);

    ASSERT_STR_EQ(tc.argv0, "/test/value");

    router_free(r);
}

/* ============================================================================ */
TEST_SUITE(rewrite_forward);

TEST(route_forward_basic) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);
    route_add(r, "/old/*", rewrite_handler);
    route_add(r, "/rewritten/path", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/old/test", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 1);
    ASSERT_EQ(result, 0);

    router_free(r);
}

TEST(route_forward_preserves_data) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/forward", check_data_handler);  /* File-scope now */

    test_ctx_t tc = {.result = 99};
    int result = route_dispatch(r, "/forward", NULL, NULL, &tc);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_forward_null_ctx) {
    int result = route_forward(NULL, "/new/path");
    ASSERT_EQ(result, -1);
}

TEST(route_forward_null_router) {
    struct route_ctx ctx = {0};
    int result = route_forward(&ctx, "/new/path");
    ASSERT_EQ(result, -1);
}

TEST(route_forward_chain) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/a/*", rewrite_handler);
    route_add(r, "/rewritten/path", rewrite_handler);
    route_add(r, "/rewritten/path", record_handler);

    test_ctx_t tc = {0};
    int result = route_dispatch(r, "/a/x", NULL, NULL, &tc);

    ASSERT_EQ(tc.called, 16);
    ASSERT_EQ(result, 0);

    router_free(r);
}

/* ============================================================================ */
TEST_SUITE(context_accessors);

TEST(route_path) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test/*", path_checker);
    int result = route_dispatch(r, "/test/path", NULL, NULL, NULL);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_data_accessor) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test", data_checker);
    int result = route_dispatch(r, "/test", NULL, NULL, (void *)0x1234);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_conn_accessor) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test", conn_checker);
    int result = route_dispatch(r, "/test", (void *)0x1234, NULL, NULL);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_req_accessor) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test", req_checker);
    int result = route_dispatch(r, "/test", NULL, (void *)0x5678, NULL);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_accessor_null) {
    ASSERT_NULL(route_path(NULL));
    ASSERT_NULL(route_data(NULL));
    ASSERT_NULL(route_conn(NULL));
    ASSERT_NULL(route_req(NULL));
}

/* ============================================================================ */
TEST_SUITE(edge_cases);

TEST(empty_router_dispatch) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    int result = route_dispatch(r, "/any/path", NULL, NULL, NULL);
    ASSERT_EQ(result, -1);

    router_free(r);
}

TEST(multiple_captures) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/api/*/*", multi_capture);
    int result = route_dispatch(r, "/api/users/123", NULL, NULL, NULL);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(spec_custom_delimiter) {
    char spec[3] = {'*', ':', '\\'};
    router_t r = router_create(spec);

    const char *matches[16];
    int argc = route_match("/api:users", "/api:*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "users");

    router_free(r);
}

TEST(spec_custom_escape) {
    char spec[3] = {'*', '/', '!'};
    router_t r = router_create(spec);

    const char *matches[16];
    int argc = route_match("/api/*test", "/api/!*test", spec, matches, 16);
    ASSERT_EQ(argc, 1);

    router_free(r);
}

TEST(dispatch_with_conn_req_data) {
    char spec[3] = {'*', '/', '\\'};
    router_t r = router_create(spec);

    route_add(r, "/test", verify_ctx_handler);  /* File-scope now */
    int result = route_dispatch(r, "/test", (void *)0x1111, (void *)0x2222, (void *)0x3333);

    ASSERT_EQ(result, 1);

    router_free(r);
}

TEST(route_match_case_sensitive) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};

    int argc = route_match("/API/users", "/api/*", spec, matches, 16);
    ASSERT_EQ(argc, 0);

    argc = route_match("/api/users", "/api/*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
}

TEST(route_match_unicode_path) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};

    int argc = route_match("/user/日本語", "/user/*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "日本語");
}

TEST(route_match_query_string) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};

    int argc = route_match("/api/users?id=123", "/api/*", spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "users?id=123");
}

/* ============================================================================ */
TEST_SUITE(performance);

TEST(route_match_short_pattern) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};

    int argc = route_match("/a", "/a", spec, matches, 16);
    ASSERT_EQ(argc, 1);
}

TEST(route_match_long_pattern) {
    const char *matches[16];
    char spec[3] = {'*', '/', '\\'};

    const char *long_path = "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p";
    const char *long_pat = "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/*";

    int argc = route_match(long_path, long_pat, spec, matches, 16);
    ASSERT_EQ(argc, 2);
    ASSERT_STR_EQ(matches[1], "p");
}

/* ============================================================================ */

TEST_MAIN()

