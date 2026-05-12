/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <transit/proto.h>

TEST_TYPE(unit);
TEST_UNIT(transit/proto.h);

/* ============================================================================ */
/* Mock Protocol Definitions (for testing)                                      */
/* ============================================================================ */

/* Storage structs (the actual data) */
static struct proto _proto_http_storage;
static struct proto _proto_ws_storage;
static struct proto _proto_sse_storage;
static struct proto _proto_http2_storage;

/* Pointers to storage (what proto_t is) */
static proto_t _proto_http = &_proto_http_storage;
static proto_t _proto_ws = &_proto_ws_storage;
static proto_t _proto_sse = &_proto_sse_storage;
static proto_t _proto_http2 = &_proto_http2_storage;

/* Def functions (return the pointer) */
static const proto_t proto_http_def(void) { return _proto_http; }
static const proto_t proto_ws_def(void) { return _proto_ws; }
static const proto_t proto_sse_def(void) { return _proto_sse; }
static const proto_t proto_http2_def(void) { return _proto_http2; }

/* Initialize storage */
static struct proto _proto_http_storage = {
    .name = "http",
    .def = proto_http_def,
    .from = NULL,
    .serve_open = NULL,
    .serve_in = NULL,
    .serve_out = NULL,
    .serve_close = NULL,
    .client_open = NULL,
    .client_in = NULL,
    .client_out = NULL,
    .client_close = NULL,
    .persist = true,
    .data = NULL
};

static struct proto _proto_ws_storage = {
    .name = "websocket",
    .def = proto_ws_def,
    .from = proto_http_def,
    .serve_open = NULL,
    .serve_in = NULL,
    .serve_out = NULL,
    .serve_close = NULL,
    .client_open = NULL,
    .client_in = NULL,
    .client_out = NULL,
    .client_close = NULL,
    .persist = true,
    .data = NULL
};

static struct proto _proto_sse_storage = {
    .name = "sse",
    .def = proto_sse_def,
    .from = proto_http_def,
    .serve_open = NULL,
    .serve_in = NULL,
    .serve_out = NULL,
    .serve_close = NULL,
    .client_open = NULL,
    .client_in = NULL,
    .client_out = NULL,
    .client_close = NULL,
    .persist = true,
    .data = NULL
};

static struct proto _proto_http2_storage = {
    .name = "http2",
    .def = proto_http2_def,
    .from = proto_http_def,
    .serve_open = NULL,
    .serve_in = NULL,
    .serve_out = NULL,
    .serve_close = NULL,
    .client_open = NULL,
    .client_in = NULL,
    .client_out = NULL,
    .client_close = NULL,
    .persist = true,
    .data = NULL
};

/* ============================================================================ */
TEST_SUITE(lifecycle);

TEST(proto_list_create) {
    proto_list_t pl = proto_list();
    ASSERT_NOT_NULL(pl);
    ASSERT_EQ(pl->length, 0);
    ASSERT_NULL(pl->main);
    proto_free(pl);
}

TEST(proto_list_free_null) {
    proto_free(NULL);
    ASSERT_TRUE(1);
}

TEST(proto_list_multiple) {
    proto_list_t pl1 = proto_list();
    proto_list_t pl2 = proto_list();

    ASSERT_NOT_NULL(pl1);
    ASSERT_NOT_NULL(pl2);
    ASSERT_PTR_NE(pl1, pl2);

    proto_free(pl1);
    proto_free(pl2);
}

/* ============================================================================ */
TEST_SUITE(registration);

TEST(proto_add_http) {
    proto_list_t pl = proto_list();
    proto_t p = proto_add(pl, proto_http_def);

    ASSERT_NOT_NULL(p);
    ASSERT_EQ(pl->length, 1);
    ASSERT_STR_EQ(p->name, "http");
    ASSERT_NULL(p->from);
    ASSERT_PTR_EQ(pl->main, p);

    proto_free(pl);
}

TEST(proto_add_ws) {
    proto_list_t pl = proto_list();

    proto_add(pl, proto_http_def);
    proto_t p = proto_add(pl, proto_ws_def);

    ASSERT_NOT_NULL(p);
    ASSERT_EQ(pl->length, 2);
    ASSERT_STR_EQ(p->name, "websocket");
    ASSERT_PTR_EQ(p->from, proto_http_def);

    proto_free(pl);
}

TEST(proto_add_duplicate) {
    proto_list_t pl = proto_list();

    proto_t p1 = proto_add(pl, proto_http_def);
    proto_t p2 = proto_add(pl, proto_http_def);

    ASSERT_PTR_EQ(p1, p2);
    ASSERT_EQ(pl->length, 1);

    proto_free(pl);
}

TEST(proto_add_null_params) {
    proto_list_t pl = proto_list();

    proto_t p = proto_add(NULL, proto_http_def);
    ASSERT_NULL(p);

    p = proto_add(pl, NULL);
    ASSERT_NULL(p);

    proto_free(pl);
}

TEST(proto_add_multiple_times) {
    proto_list_t pl = proto_list();

    for (int i = 0; i < PROTO_LIST_MAX + 10; i++) {
        proto_add(pl, proto_http_def);
    }

    ASSERT_EQ(pl->length, 1);

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(lookup);

TEST(proto_main) {
    proto_list_t pl = proto_list();

    ASSERT_NULL(proto_main(pl));

    proto_add(pl, proto_http_def);
    proto_t main = proto_main(pl);

    ASSERT_NOT_NULL(main);
    ASSERT_STR_EQ(main->name, "http");

    proto_free(pl);
}

TEST(proto_find) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    proto_t p = proto_find(pl, proto_http_def);
    ASSERT_NOT_NULL(p);
    ASSERT_STR_EQ(p->name, "http");

    p = proto_find(pl, proto_ws_def);
    ASSERT_NOT_NULL(p);
    ASSERT_STR_EQ(p->name, "websocket");

    p = proto_find(pl, proto_sse_def);
    ASSERT_NULL(p);

    proto_free(pl);
}

TEST(proto_find_null_params) {
    proto_list_t pl = proto_list();

    ASSERT_NULL(proto_find(NULL, proto_http_def));
    ASSERT_NULL(proto_find(pl, NULL));

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(upgrade);

TEST(proto_upgrade_valid) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    proto_t http = proto_find(pl, proto_http_def);
    proto_t ws = proto_upgrade(pl, http, "websocket");

    ASSERT_NOT_NULL(ws);
    ASSERT_STR_EQ(ws->name, "websocket");
    ASSERT_PTR_EQ(ws->from, proto_http_def);

    proto_free(pl);
}

TEST(proto_upgrade_invalid_name) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    proto_t http = proto_find(pl, proto_http_def);
    proto_t invalid = proto_upgrade(pl, http, "nonexistent");

    ASSERT_NULL(invalid);

    proto_free(pl);
}

TEST(proto_upgrade_wrong_parent) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);
    proto_add(pl, proto_sse_def);

    proto_t ws = proto_find(pl, proto_ws_def);
    proto_t sse = proto_upgrade(pl, ws, "sse");

    ASSERT_NULL(sse);

    proto_free(pl);
}

TEST(proto_upgrade_null_params) {
    proto_list_t pl = proto_list();

    ASSERT_NULL(proto_upgrade(NULL, NULL, "http"));
    ASSERT_NULL(proto_upgrade(pl, NULL, "http"));
    ASSERT_NULL(proto_upgrade(pl, _proto_http, NULL));

    proto_free(pl);
}

TEST(proto_upgrade_chain) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    proto_t http = proto_find(pl, proto_http_def);
    proto_t ws = proto_upgrade(pl, http, "websocket");

    proto_t next = proto_upgrade(pl, ws, "http2");

    ASSERT_NULL(next);

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(removal);

TEST(proto_remove) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    ASSERT_EQ(pl->length, 2);

    proto_remove(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 1);

    proto_t p = proto_find(pl, proto_ws_def);
    ASSERT_NULL(p);

    proto_free(pl);
}

TEST(proto_remove_not_found) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);

    proto_remove(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 1);

    proto_free(pl);
}

TEST(proto_remove_null_params) {
    proto_list_t pl = proto_list();

    proto_remove(NULL, proto_http_def);
    proto_remove(pl, NULL);

    proto_free(pl);
}

TEST(proto_remove_main) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    ASSERT_NOT_NULL(pl->main);

    proto_remove(pl, proto_http_def);
    ASSERT_NULL(pl->main);

    proto_free(pl);
}

TEST(proto_remove_multiple_same) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);
    proto_add(pl, proto_ws_def);

    proto_remove(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 1);

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(self_reference);

TEST(proto_def_returns_self) {
    const proto_t p = proto_http_def();
    ASSERT_PTR_EQ(p, _proto_http);
    ASSERT_PTR_EQ(p->def(), p);
}

TEST(proto_from_chain) {
    const proto_t ws = proto_ws_def();
    ASSERT_PTR_EQ(ws->from, proto_http_def);

    const proto_t http = proto_http_def();
    ASSERT_NULL(http->from);
}

/* ============================================================================ */
TEST_SUITE(metadata);

TEST(proto_persist_flag) {
    ASSERT_TRUE(_proto_http->persist);
    ASSERT_TRUE(_proto_ws->persist);
    ASSERT_TRUE(_proto_sse->persist);
}

TEST(proto_data_pointer) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);

    proto_t p = proto_find(pl, proto_http_def);
    ASSERT_NULL(p->data);

    int my_data = 42;
    p->data = &my_data;
    ASSERT_PTR_EQ(p->data, &my_data);

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(edge_cases);

TEST(proto_empty_name) {
    struct proto proto = {
        .name = "",
        .def = NULL,
        .from = NULL,
        .persist = false
    };

    ASSERT_STR_EQ(proto.name, "");
}

TEST(proto_list_zeroed) {
    proto_list_t pl = proto_list();

    for (int i = 0; i < pl->length; i++) {
        ASSERT_NULL(pl->proto[i]);
    }

    proto_free(pl);
}

TEST(proto_upgrade_case_sensitive) {
    proto_list_t pl = proto_list();
    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);

    proto_t http = proto_find(pl, proto_http_def);

    proto_t ws = proto_upgrade(pl, http, "WebSocket");
    ASSERT_NULL(ws);

    ws = proto_upgrade(pl, http, "websocket");
    ASSERT_NOT_NULL(ws);

    proto_free(pl);
}

/* ============================================================================ */
TEST_SUITE(integration);

TEST(proto_full_registration_chain) {
    proto_list_t pl = proto_list();

    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);
    proto_add(pl, proto_sse_def);
    proto_add(pl, proto_http2_def);

    ASSERT_EQ(pl->length, 4);
    ASSERT_NOT_NULL(pl->main);
    ASSERT_STR_EQ(pl->main->name, "http");

    ASSERT_NOT_NULL(proto_find(pl, proto_http_def));
    ASSERT_NOT_NULL(proto_find(pl, proto_ws_def));
    ASSERT_NOT_NULL(proto_find(pl, proto_sse_def));
    ASSERT_NOT_NULL(proto_find(pl, proto_http2_def));

    proto_t http = proto_find(pl, proto_http_def);
    ASSERT_NOT_NULL(proto_upgrade(pl, http, "websocket"));
    ASSERT_NOT_NULL(proto_upgrade(pl, http, "sse"));
    ASSERT_NOT_NULL(proto_upgrade(pl, http, "http2"));

    proto_free(pl);
}

TEST(proto_remove_then_readd) {
    proto_list_t pl = proto_list();

    proto_add(pl, proto_http_def);
    proto_add(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 2);

    proto_remove(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 1);

    proto_add(pl, proto_ws_def);
    ASSERT_EQ(pl->length, 2);

    proto_free(pl);
}

/* ============================================================================ */

TEST_MAIN()
