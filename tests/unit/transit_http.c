/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <transit/http.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(proto/http.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_http_methods) {
    ASSERT_EQ(HTTP_GET, 0);
    ASSERT_EQ(HTTP_POST, 1);
    ASSERT_EQ(HTTP_PUT, 2);
    ASSERT_EQ(HTTP_DELETE, 3);
    ASSERT_EQ(HTTP_HEAD, 4);
    ASSERT_EQ(HTTP_OPTIONS, 5);
    ASSERT_EQ(HTTP_PATCH, 6);
    ASSERT_EQ(HTTP_CONNECT, 7);
    ASSERT_EQ(HTTP_UNKNOWN, 99);
}

TEST(constants_http_codes) {
    ASSERT_EQ(HTTP_CONT, 100);
    ASSERT_EQ(HTTP_SWITCH, 101);
    ASSERT_EQ(HTTP_OK, 200);
    ASSERT_EQ(HTTP_CREATED, 201);
    ASSERT_EQ(HTTP_NO_CONTENT, 204);
    ASSERT_EQ(HTTP_MOVED, 301);
    ASSERT_EQ(HTTP_FOUND, 302);
    ASSERT_EQ(HTTP_NOT_MODIFIED, 304);
    ASSERT_EQ(HTTP_UNAUTHORIZED, 401);
    ASSERT_EQ(HTTP_FORBIDDEN, 403);
    ASSERT_EQ(HTTP_NOT_FOUND, 404);
    ASSERT_EQ(HTTP_METHOD, 405);
    ASSERT_EQ(HTTP_ERROR, 400);
    ASSERT_EQ(HTTP_NOT_IMPL, 501);
    ASSERT_EQ(HTTP_BAD_GATE, 502);
    ASSERT_EQ(HTTP_UNAVAILABLE, 503);
}

TEST(constants_http_status) {
    ASSERT_EQ(HSTAT_HEADERS, 0);
    ASSERT_EQ(HSTAT_VERSION, 1);
    ASSERT_EQ(HSTAT_MORE, 2);
    ASSERT_EQ(HSTAT_ERROR, 3);
    ASSERT_EQ(HSTAT_UPGRADE, 4);
}

/* ============================================================================ */
TEST_SUITE(structures);

TEST(struct_http_hdr_t_zero_copy) {
    /* Verify header struct is two pointers (zero-copy design) */
    ASSERT_EQ(sizeof(http_hdr_t), sizeof(void*) * 2);
}

TEST(struct_http_req_t_zero_copy_fields) {
    http_req_t req;
    memset(&req, 0, sizeof(req));

    /* Critical: these must be pointers, not buffers */
    ASSERT_SIZE_SAME(req.path, const char*);
    ASSERT_SIZE_SAME(req.query, const char*);
    ASSERT_SIZE_SAME(req.authority, const char*);
    ASSERT_SIZE_SAME(req.upgrade_proto, const char*);
}

TEST(struct_http_req_t_header_array) {
    http_req_t req;
    ASSERT_SIZE(req.hdrs, sizeof(http_hdr_t) * HTTP_MAXHDR);
    ASSERT_EQ(req.hcount, 0);
}

/* ============================================================================ */
TEST_SUITE(version_detection);

TEST(http_detect_version_h2_alpn) {
    uint16_t ver = http_detect_version("h2", NULL, 0);
    ASSERT_EQ(ver, HTTP_VERSION_20);
}

TEST(http_detect_version_h3_alpn) {
    uint16_t ver = http_detect_version("h3", NULL, 0);
    ASSERT_EQ(ver, HTTP_VERSION_30);
}

TEST(http2_is_preface_valid) {
    const uint8_t preface[] = HTTP2_PREFACE;
    ASSERT_TRUE(http2_is_preface(preface, HTTP2_PREFACE_LEN));
}

/* ============================================================================ */
TEST_SUITE(parser_init);

TEST(http_init_zeroes_struct) {
    http_req_t req;
    uint8_t buf[2048];

    memset(&req, 0xAA, sizeof(req));
    http_init(&req, buf, sizeof(buf));

    ASSERT_EQ(req.method, HTTP_UNKNOWN);
    ASSERT_NULL(req.path);
    ASSERT_EQ(req.hcount, 0);
    ASSERT_EQ(req.content_len, 0);
    ASSERT_TRUE(req.keep_alive);
}

/* ============================================================================ */
TEST_SUITE(parser_feed_http11);

TEST(http_feed_simple_get) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /path HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_HEADERS);
    ASSERT_EQ(req.method, HTTP_GET);
    ASSERT_STR_EQ("/path", req.path);
    ASSERT_STR_EQ("example.com", http_get_header(&req, "Host"));
}

TEST(http_feed_with_query) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /search?q=test&lang=en HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_HEADERS);
    ASSERT_STR_EQ("/search", req.path);
    ASSERT_STR_EQ("q=test&lang=en", req.query);
}

TEST(http_feed_multiple_headers) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "POST /api HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Content-Type: application/json\r\n"
                         "Authorization: Bearer token123\r\n"
                         "Content-Length: 15\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_HEADERS);
    ASSERT_EQ(req.method, HTTP_POST);
    ASSERT_EQ(req.hcount, 4);
    ASSERT_EQ(req.content_len, 15);
    ASSERT_STR_EQ("application/json", http_get_header(&req, "Content-Type"));
}

TEST(http_feed_chunked_encoding) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "POST /upload HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Transfer-Encoding: chunked\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_HEADERS);
    ASSERT_TRUE(req.chunked);
    ASSERT_EQ(req.content_len, 0);
}

TEST(http_feed_connection_close) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET / HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Connection: close\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_FALSE(req.keep_alive);
}

TEST(http_feed_upgrade_websocket) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /ws HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_UPGRADE);
    ASSERT_TRUE(req.upgrade);
    ASSERT_STR_EQ("websocket", req.upgrade_proto);
}

TEST(http_feed_malformed_request) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET/path HTTP/1.1\r\n\r\n";
    memcpy(buf, request, strlen(request));

    http_stat_t st = http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(st, HSTAT_ERROR);
}

/* ============================================================================ */
TEST_SUITE(parser_zero_copy);

TEST(http_feed_pointers_into_buffer) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /test HTTP/1.1\r\n"
                         "X-Custom: value123\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_feed(&req, buf, strlen(request), &consumed);

    /* Critical: pointers must be INTO buf, not copies */
    ASSERT_TRUE(req.path >= (char*)buf && req.path < (char*)buf + sizeof(buf));
    ASSERT_TRUE(req.hdrs[0].key >= (char*)buf && req.hdrs[0].key < (char*)buf + sizeof(buf));
    ASSERT_TRUE(req.hdrs[0].val >= (char*)buf && req.hdrs[0].val < (char*)buf + sizeof(buf));
}

TEST(http_feed_mutation_creates_cstrings) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /test HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_feed(&req, buf, strlen(request), &consumed);

    /* Parser must mutate \r\n to \0, making C-strings */
    ASSERT_EQ(strlen(req.path), 5);
    ASSERT_EQ(strlen(http_get_header(&req, "Host")), 11);
}

TEST(http_get_header_case_insensitive) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET / HTTP/1.1\r\n"
                         "CONTENT-TYPE: application/json\r\n"
                         "authorization: Bearer token\r\n"
                         "\r\n";
    memcpy(buf, request, strlen(request));

    http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_STR_EQ("application/json", http_get_header(&req, "Content-Type"));
    ASSERT_STR_EQ("application/json", http_get_header(&req, "content-type"));
    ASSERT_STR_EQ("Bearer token", http_get_header(&req, "Authorization"));
}

/* ============================================================================ */
TEST_SUITE(parser_reset);

TEST(http_reset_clears_parsed_state) {
    http_req_t req;
    uint8_t buf[2048];
    size_t consumed;

    http_init(&req, buf, sizeof(buf));

    const char *request = "GET /first HTTP/1.1\r\n\r\n";
    memcpy(buf, request, strlen(request));
    http_feed(&req, buf, strlen(request), &consumed);

    ASSERT_EQ(req.method, HTTP_GET);
    ASSERT_STR_EQ("/first", req.path);

    http_reset(&req);

    ASSERT_EQ(req.method, HTTP_UNKNOWN);
    ASSERT_NULL(req.path);
    ASSERT_EQ(req.hcount, 0);
}

/* ============================================================================ */
TEST_SUITE(response_builders);

TEST(res_init_sets_code_and_reason) {
    http_res_t res;

    http_res_init(&res, HTTP_OK, "OK");

    ASSERT_EQ(res.code, HTTP_OK);
    ASSERT_STR_EQ("OK", res.reason);
}

TEST(res_header_appends_to_buffer) {
    http_res_t res;

    http_res_init(&res, HTTP_OK, "OK");

    ASSERT_EQ(http_res_header(&res, "Content-Type", "text/html"), 0);
    ASSERT_EQ(http_res_header(&res, "Cache-Control", "no-cache"), 0);

    ASSERT_STR_HAS("Content-Type: text/html", res.hdr_buf);
    ASSERT_STR_HAS("Cache-Control: no-cache", res.hdr_buf);
}

TEST(res_body_sets_pointers) {
    http_res_t res;
    uint8_t body[] = "Hello, World!";

    http_res_init(&res, HTTP_OK, "OK");
    http_res_body(&res, body, sizeof(body) - 1);

    ASSERT_PTR_EQ(res.body, body);
    ASSERT_EQ(res.body_len, sizeof(body) - 1);
    ASSERT_NULL(res.file_path);
}

TEST(res_build_simple_response) {
    http_res_t res;
    uint8_t out[1024];

    http_res_init(&res, HTTP_OK, "OK");
    http_res_header(&res, "Content-Length", "5");
    http_res_body(&res, (uint8_t*)"Hello", 5);

    size_t len = http_res_build(out, sizeof(out), &res);

    ASSERT_TRUE(len > 0);
    ASSERT_STR_HAS("HTTP/1.1 200 OK", (char*)out);
    ASSERT_STR_HAS("Content-Length: 5", (char*)out);
    ASSERT_STR_HAS("Hello", (char*)out);
}

TEST(res_build_buffer_too_small) {
    http_res_t res;
    uint8_t tiny[10];

    http_res_init(&res, HTTP_OK, "OK");
    http_res_body(&res, (uint8_t*)"Hello, World!", 13);

    size_t len = http_res_build(tiny, sizeof(tiny), &res);

    ASSERT_EQ(len, 0);
}

/* ============================================================================ */
TEST_SUITE(utility_functions);

TEST(http_method_str) {
    ASSERT_STR_EQ("GET", http_method_str(HTTP_GET));
    ASSERT_STR_EQ("POST", http_method_str(HTTP_POST));
    ASSERT_STR_EQ("UNKNOWN", http_method_str(HTTP_UNKNOWN));
}

TEST(http_method_from_str) {
    ASSERT_EQ(http_method_from_str("GET"), HTTP_GET);
    ASSERT_EQ(http_method_from_str("POST"), HTTP_POST);
    ASSERT_EQ(http_method_from_str("INVALID"), HTTP_UNKNOWN);
}

TEST(http_code_reason) {
    ASSERT_STR_EQ("OK", http_code_reason(HTTP_OK));
    ASSERT_STR_EQ("Not Found", http_code_reason(HTTP_NOT_FOUND));
    ASSERT_STR_EQ("Bad Request", http_code_reason(HTTP_ERROR));
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "proto/http.h needs POSIX")

#else

int main(void) {
    printf("proto/http.h requires POSIX\n");
    return 0;
}

#endif
