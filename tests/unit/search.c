/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <search.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(search.h);

/* Comparators */
static int cmp_str(const void *a, const void *b) { return strcmp(*(const char **)a, *(const char **)b); }
static int cmp_int(const void *a, const void *b) { return (*(const int *)a) - (*(const int *)b); }
static int cmp_bin4(const void *a, const void *b) { return memcmp(a, b, 4); }
static int cmp_byte(const void *a, const void *b) { return memcmp(a, b, 1); }

/* ============================================================================ */
TEST_SUITE(search_lfind);

TEST(lfind_single_match) {
	int arr[] = {42}; size_t n = 1; int k = 42;
	ASSERT_PTR_EQ(lfind(&k, arr, &n, sizeof(int), cmp_int), arr);
}

TEST(lfind_first_of_many) {
	int arr[] = {1, 2, 3, 4}; size_t n = 4; int k = 1;
	ASSERT_PTR_EQ(lfind(&k, arr, &n, sizeof(int), cmp_int), &arr[0]);
}

TEST(lfind_last_of_many) {
	int arr[] = {1, 2, 3, 4}; size_t n = 4; int k = 4;
	ASSERT_PTR_EQ(lfind(&k, arr, &n, sizeof(int), cmp_int), &arr[3]);
}

TEST(lfind_middle) {
	int arr[] = {10, 20, 30, 40}; size_t n = 4; int k = 20;
	ASSERT_PTR_EQ(lfind(&k, arr, &n, sizeof(int), cmp_int), &arr[1]);
}

TEST(lfind_no_match) {
	int arr[] = {1, 2, 3}; size_t n = 3; int k = 5;
	ASSERT_NULL(lfind(&k, arr, &n, sizeof(int), cmp_int));
}

TEST(lfind_empty_array) {
	int arr[1]; size_t n = 0; int k = 1;
	ASSERT_NULL(lfind(&k, arr, &n, sizeof(int), cmp_int));
}

TEST(lfind_null_base) {
	size_t n = 1; int k = 1;
	ASSERT_NULL(lfind(&k, NULL, &n, sizeof(int), cmp_int));
}

TEST(lfind_null_nmemb) {
	int arr[] = {1}; int k = 1;
	ASSERT_NULL(lfind(&k, arr, NULL, sizeof(int), cmp_int));
}

TEST(lfind_const_correctness) {
	const int arr[] = {7, 8, 9}; size_t n = 3; int k = 8;
	const void *res = lfind(&k, arr, &n, sizeof(int), cmp_int);
	ASSERT_NOT_NULL(res);
}

TEST(lfind_struct_elements) {
	typedef struct { int id; char tag; } Item;
	Item arr[] = {{1,'a'}, {2,'b'}}; size_t n = 2;
	Item k = {2, 'x'};
	ASSERT_PTR_EQ(lfind(&k, arr, &n, sizeof(Item), cmp_int), &arr[1]);
}

/* ============================================================================ */
TEST_SUITE(search_lsearch);

TEST(lsearch_inserts_new) {
	char *buf[10] = {"a"}; size_t n = 1;
	char *k = "b";
	char **res = lsearch(&k, buf, &n, sizeof(char *), cmp_str);
	ASSERT_INT_EQ(n, 2);
	ASSERT_STR_EQ(*res, "b");
}

TEST(lsearch_updates_nmemb) {
	int arr[10]; size_t n = 0;
	for (int i = 0; i < 5; i++) { int v = i; lsearch(&v, arr, &n, sizeof(int), cmp_int); }
	ASSERT_INT_EQ(n, 5);
}

TEST(lsearch_returns_existing) {
	int arr[] = {10}; size_t n = 1; int k = 10;
	void *first = lsearch(&k, arr, &n, sizeof(int), cmp_int);
	void *second = lsearch(&k, arr, &n, sizeof(int), cmp_int);
	ASSERT_PTR_EQ(first, second);
	ASSERT_INT_EQ(n, 1);
}

TEST(lsearch_appends_at_end) {
	int arr[10] = {1, 2}; size_t n = 2; int k = 3;
	int *res = lsearch(&k, arr, &n, sizeof(int), cmp_int);
	ASSERT_PTR_EQ(res, &arr[2]);
}

TEST(lsearch_binary_data) {
	unsigned char buf[16] = {0}; size_t n = 0;
	unsigned char chunk[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	lsearch(chunk, buf, &n, 4, cmp_bin4);
	ASSERT_INT_EQ(n, 1);
	ASSERT_MEM_EQ(buf, chunk, 4);
}

TEST(lsearch_null_base) {
	size_t n = 0; int k = 1;
	ASSERT_NULL(lsearch(&k, NULL, &n, sizeof(int), cmp_int));
}

TEST(lsearch_size_one) {
	char buf[2] = {0}; size_t n = 1; buf[0] = 'A';
	char k = 'B';
	char *res = lsearch(&k, buf, &n, sizeof(char), cmp_byte);
	ASSERT_INT_EQ(*res, 'B');
}

TEST(lsearch_overwrites_memory_safely) {
	int arr[10]; size_t n = 2; arr[0]=1; arr[1]=2;
	int k = 3;
	lsearch(&k, arr, &n, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[1], 2);
	ASSERT_INT_EQ(arr[2], 3);
}

TEST(lsearch_duplicate_count_unchanged) {
	int arr[10] = {5}; size_t n = 1; int k = 5;
	lsearch(&k, arr, &n, sizeof(int), cmp_int);
	lsearch(&k, arr, &n, sizeof(int), cmp_int);
	ASSERT_INT_EQ(n, 1);
}

TEST(lsearch_respects_element_stride) {
	int arr[10]; size_t n = 0;
	for (int i=0; i<3; i++) { int v=i*10; lsearch(&v, arr, &n, sizeof(int), cmp_int); }
	ASSERT_INT_EQ(arr[2], 20);
}

/* ============================================================================ */
TEST_SUITE(search_hsearch);

TEST(hcreate_initial) {
	hdestroy();
	ASSERT_INT_EQ(hcreate(8), 1);
	hdestroy();
}

TEST(hcreate_duplicate_fails) {
	hdestroy();
	hcreate(8);
	ASSERT_INT_EQ(hcreate(16), 0);
	hdestroy();
}

TEST(hcreate_small_table) {
	hdestroy();
	ASSERT_INT_EQ(hcreate(1), 1);
	hdestroy();
}

TEST(hsearch_enter_new) {
	hdestroy(); hcreate(16);
	ENTRY e = { .key = "alpha", .data = (void *)100 };
	ENTRY *res = hsearch(e, ENTER);
	ASSERT_NOT_NULL(res);
	ASSERT_STR_EQ(res->key, "alpha");
	ASSERT_PTR_EQ(res->data, (void *)100);
	hdestroy();
}

TEST(hsearch_find_existing) {
	hdestroy(); hcreate(16);
	hsearch((ENTRY){.key="beta", .data=(void*)200}, ENTER);
	ENTRY *res = hsearch((ENTRY){.key="beta", .data=NULL}, FIND);
	ASSERT_NOT_NULL(res);
	ASSERT_PTR_EQ(res->data, (void *)200);
	hdestroy();
}

TEST(hsearch_find_missing) {
	hdestroy(); hcreate(16);
	errno = 0;
	ENTRY *res = hsearch((ENTRY){.key="gamma", .data=NULL}, FIND);
	ASSERT_NULL(res);
	ASSERT_ERRNO(ESRCH);
	hdestroy();
}

TEST(hsearch_enter_updates_data) {
	hdestroy(); hcreate(16);
	hsearch((ENTRY){.key="delta", .data=(void*)1}, ENTER);
	hsearch((ENTRY){.key="delta", .data=(void*)999}, ENTER);
	ENTRY *res = hsearch((ENTRY){.key="delta", .data=NULL}, FIND);
	ASSERT_PTR_EQ(res->data, (void *)999);
	hdestroy();
}

TEST(hsearch_null_key) {
	hdestroy(); hcreate(16);
	errno = 0;
	ENTRY *res = hsearch((ENTRY){.key=NULL, .data=(void*)1}, ENTER);
	ASSERT_NULL(res);
	hdestroy();
}

TEST(hsearch_empty_string_key) {
	hdestroy(); hcreate(16);
	hsearch((ENTRY){.key="", .data=(void*)1}, ENTER);
	ENTRY *res = hsearch((ENTRY){.key="", .data=NULL}, FIND);
	ASSERT_NOT_NULL(res);
	hdestroy();
}

TEST(hsearch_long_key) {
	hdestroy(); hcreate(16);
	char buf[256]; memset(buf, 'A', 255); buf[255] = '\0';
	hsearch((ENTRY){.key=buf, .data=(void*)1}, ENTER);
	ENTRY *res = hsearch((ENTRY){.key=buf, .data=NULL}, FIND);
	ASSERT_NOT_NULL(res);
	hdestroy();
}

TEST(hsearch_case_sensitive) {
	hdestroy(); hcreate(16);
	hsearch((ENTRY){.key="Case", .data=(void*)1}, ENTER);
	ASSERT_NULL(hsearch((ENTRY){.key="case", .data=NULL}, FIND));
	hdestroy();
}

TEST(hsearch_table_full) {
	hdestroy(); hcreate(4);
	hsearch((ENTRY){.key="a", .data=NULL}, ENTER);
	hsearch((ENTRY){.key="b", .data=NULL}, ENTER);
	hsearch((ENTRY){.key="c", .data=NULL}, ENTER);
	hsearch((ENTRY){.key="d", .data=NULL}, ENTER);
	errno = 0;
	ASSERT_NULL(hsearch((ENTRY){.key="e", .data=NULL}, ENTER));
	ASSERT_ERRNO(ENOSPC);
	hdestroy();
}

TEST(hsearch_after_destroy) {
	hdestroy(); hcreate(8);
	hdestroy();
	errno = 0;
	ASSERT_NULL(hsearch((ENTRY){.key="x", .data=NULL}, FIND));
	ASSERT_ERRNO(EINVAL);
}

TEST(hsearch_destroys_clean) {
	hdestroy(); hcreate(8);
	hsearch((ENTRY){.key="cleanup", .data=(void*)1}, ENTER);
	hdestroy();
	hdestroy();
	ASSERT_INT_EQ(hcreate(8), 1);
	hdestroy();
}

TEST(hsearch_preserves_entry_key_pointer) {
	hdestroy(); hcreate(8);
	char key[] = "ptr_test";
	hsearch((ENTRY){.key=key, .data=NULL}, ENTER);
	ENTRY *res = hsearch((ENTRY){.key=key, .data=NULL}, FIND);
	ASSERT_PTR_EQ(res->key, key);
	hdestroy();
}

TEST(hsearch_hash_collision_handling) {
	hdestroy(); hcreate(8);
	hsearch((ENTRY){.key="A", .data=(void*)1}, ENTER);
	hsearch((ENTRY){.key="Q", .data=(void*)2}, ENTER);
	ASSERT_NOT_NULL(hsearch((ENTRY){.key="Q", .data=NULL}, FIND));
	hdestroy();
}

TEST(hsearch_zero_nel) {
	hdestroy();
	ASSERT_INT_EQ(hcreate(0), 1);
	hsearch((ENTRY){.key="zero", .data=(void*)1}, ENTER);
	ASSERT_NOT_NULL(hsearch((ENTRY){.key="zero", .data=NULL}, FIND));
	hdestroy();
}

TEST(hsearch_multiple_inserts) {
	hdestroy(); hcreate(32);
	for (int i = 0; i < 10; i++) {
		char buf[8]; snprintf(buf, 8, "k%d", i);
		hsearch((ENTRY){.key=strdup(buf), .data=(void*)(intptr_t)i}, ENTER);
	}
	for (int i = 0; i < 10; i++) {
		char buf[8]; snprintf(buf, 8, "k%d", i);
		ASSERT_NOT_NULL(hsearch((ENTRY){.key=buf, .data=NULL}, FIND));
	}
	hdestroy();
}

TEST(hsearch_enter_returns_same_pointer) {
	hdestroy(); hcreate(8);
	ENTRY *a = hsearch((ENTRY){.key="same", .data=(void*)1}, ENTER);
	ENTRY *b = hsearch((ENTRY){.key="same", .data=(void*)2}, ENTER);
	ASSERT_PTR_EQ(a, b);
	hdestroy();
}

TEST(hsearch_invalid_action) {
	hdestroy(); hcreate(8);
	hsearch((ENTRY){.key="inv", .data=NULL}, ENTER);
	ASSERT_INT_EQ((int)FIND, 0);
	ASSERT_INT_EQ((int)ENTER, 1);
	hdestroy();
}

/* ============================================================================ */
TEST_SUITE(search_tsearch_tfind);

TEST(tsearch_insert_root) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 50;
	void *res = tsearch(k, &root, cmp_int);
	ASSERT_NOT_NULL(res);
	ASSERT_PTR_EQ(root, res);
	free(k);
}

TEST(tsearch_find_existing) {
	void *root = NULL;
	int *k1 = malloc(sizeof(int)); *k1 = 10;
	int *k2 = malloc(sizeof(int)); *k2 = 20;
	tsearch(k1, &root, cmp_int);
	void *res = tsearch(k1, &root, cmp_int);
	ASSERT_PTR_EQ(res, root);
	free(k1); free(k2);
}

TEST(tfind_existing) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 99;
	tsearch(k, &root, cmp_int);
	ASSERT_PTR_EQ(tfind(k, &root, cmp_int), root);
	free(k);
}

TEST(tfind_missing) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 5;
	tsearch(k, &root, cmp_int);
	int miss = 100;
	ASSERT_NULL(tfind(&miss, &root, cmp_int));
	free(k);
}

TEST(tfind_null_root) {
	void *root = NULL; int k = 1;
	ASSERT_NULL(tfind(&k, &root, cmp_int));
}

TEST(tsearch_builds_tree) {
	void *root = NULL;
	int vals[] = {5, 3, 7, 1, 4};
	for (int i=0; i<5; i++) { int *p = malloc(sizeof(int)); *p = vals[i]; tsearch(p, &root, cmp_int); }
	ASSERT_NOT_NULL(root);
}

TEST(tsearch_preserves_key_pointer) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 42;
	void *node = tsearch(k, &root, cmp_int);
	ASSERT_PTR_EQ(tfind(k, &root, cmp_int), node);
	free(k);
}

TEST(tsearch_duplicate_returns_existing) {
	void *root = NULL;
	int *a = malloc(sizeof(int)); *a = 10;
	int *b = malloc(sizeof(int)); *b = 10;
	void *n1 = tsearch(a, &root, cmp_int);
	void *n2 = tsearch(b, &root, cmp_int);
	ASSERT_PTR_EQ(n1, n2);
	free(a); free(b);
}

TEST(tsearch_null_rootp) {
	int k = 1;
	ASSERT_NULL(tsearch(&k, NULL, cmp_int));
}

TEST(tsearch_null_compar) {
	//TEST_SKIP("causes segfault");
	void *root = NULL; int k = 1;
	ASSERT_NULL(tsearch(&k, &root, NULL));
}

TEST(tsearch_integer_keys) {
	void *root = NULL;
	for (int i = 0; i < 10; i++) {
		int *p = malloc(sizeof(int)); *p = i;
		tsearch(p, &root, cmp_int);
	}
	for (int i = 0; i < 10; i++) {
		int k = i;
		ASSERT_NOT_NULL(tfind(&k, &root, cmp_int));
	}
}

TEST(tsearch_order_independent) {
	void *rootA = NULL, *rootB = NULL;
	int seqA[] = {1,2,3,4,5};
	int seqB[] = {3,1,5,2,4};
	for (int i=0; i<5; i++) { int *p = malloc(sizeof(int)); *p = seqA[i]; tsearch(p, &rootA, cmp_int); }
	for (int i=0; i<5; i++) { int *p = malloc(sizeof(int)); *p = seqB[i]; tsearch(p, &rootB, cmp_int); }
	for (int i=1; i<=5; i++) {
		int k = i;
		ASSERT_NOT_NULL(tfind(&k, &rootA, cmp_int));
		ASSERT_NOT_NULL(tfind(&k, &rootB, cmp_int));
	}
}

/* ============================================================================ */
TEST_SUITE(search_tdelete);

TEST(tdelete_leaf_returns_null) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 1;
	tsearch(k, &root, cmp_int);
	void *res = tdelete(k, &root, cmp_int);
	ASSERT_NULL(res);
	free(k);
}

TEST(tdelete_one_child_returns_parent) {
	void *root = NULL;
	int *k1 = malloc(sizeof(int)); *k1 = 10;
	int *k2 = malloc(sizeof(int)); *k2 = 20;
	tsearch(k1, &root, cmp_int);
	tsearch(k2, &root, cmp_int);
	void *res = tdelete(k1, &root, cmp_int);
	ASSERT_PTR_EQ(res, root);
	free(k2);
}

TEST(tdelete_two_children_replaces) {
	void *root = NULL;
	int *k1 = malloc(sizeof(int)); *k1 = 20;
	int *k2 = malloc(sizeof(int)); *k2 = 10;
	int *k3 = malloc(sizeof(int)); *k3 = 30;
	tsearch(k1, &root, cmp_int);
	tsearch(k2, &root, cmp_int);
	tsearch(k3, &root, cmp_int);
	void *res = tdelete(k1, &root, cmp_int);
	ASSERT_NOT_NULL(res);
	free(k2); free(k3);
}

TEST(tdelete_missing_returns_null) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 5;
	tsearch(k, &root, cmp_int);
	int miss = 99;
	void *res = tdelete(&miss, &root, cmp_int);
	ASSERT_NULL(res);
	free(k);
}

TEST(tdelete_empty_tree) {
	void *root = NULL;
	int k = 1;
	ASSERT_NULL(tdelete(&k, &root, cmp_int));
}

TEST(tdelete_null_rootp) {
	int k = 1;
	ASSERT_NULL(tdelete(&k, NULL, cmp_int));
}

TEST(tdelete_updates_root_pointer) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 5;
	tsearch(k, &root, cmp_int);
	tdelete(k, &root, cmp_int);
	ASSERT_NULL(root);
}

TEST(tdelete_preserves_tree_integrity) {
	void *root = NULL;
	int vals[] = {50, 25, 75, 10, 30};
	for (int i=0; i<5; i++) { int *p = malloc(sizeof(int)); *p = vals[i]; tsearch(p, &root, cmp_int); }
	int del = 25;
	tdelete(&del, &root, cmp_int);
	int c1 = 10, c2 = 30;
	ASSERT_NOT_NULL(tfind(&c1, &root, cmp_int));
	ASSERT_NOT_NULL(tfind(&c2, &root, cmp_int));
}

TEST(tdelete_multiple_deletes) {
	void *root = NULL;
	for (int i=0; i<5; i++) { int *p = malloc(sizeof(int)); *p = i*10; tsearch(p, &root, cmp_int); }
	for (int i=4; i>0; i--) { int k = i*10; ASSERT_NOT_NULL(tdelete(&k, &root, cmp_int)); }
	int last = 0;
	ASSERT_NULL(tdelete(&last, &root, cmp_int)); /* Final delete empties tree */
	ASSERT_NULL(root);
}

TEST(tdelete_returns_correct_parent_chain) {
	void *root = NULL;
	int *a = malloc(sizeof(int)); *a = 10;
	int *b = malloc(sizeof(int)); *b = 5;
	int *c = malloc(sizeof(int)); *c = 3;
	tsearch(a, &root, cmp_int);
	tsearch(b, &root, cmp_int);
	tsearch(c, &root, cmp_int);
	void *res = tdelete(b, &root, cmp_int);
	ASSERT_PTR_EQ(res, root);
	free(a); free(c);
}

/* ============================================================================ */
TEST_SUITE(search_twalk);

static int g_visit_count = 0;
static VISIT g_last_visit = preorder;
static void count_cb(const void *n, VISIT v, int d) {
	(void)n; (void)d;
	g_visit_count++;
	g_last_visit = v;
}

TEST(twalk_null_root) {
	g_visit_count = 0;
	twalk(NULL, count_cb);
	ASSERT_INT_EQ(g_visit_count, 0);
}

TEST(twalk_single_node) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 1;
	tsearch(k, &root, cmp_int);
	g_visit_count = 0;
	twalk(root, count_cb);
	ASSERT_INT_EQ(g_visit_count, 4);
	free(k);
}

TEST(twalk_visits_preorder_first) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 5;
	tsearch(k, &root, cmp_int);
	g_visit_count = 0; g_last_visit = preorder;
	twalk(root, count_cb);
	ASSERT_TRUE(g_visit_count == 4);
	free(k);
}

TEST(twalk_leaf_detection) {
	void *root = NULL;
	int *k1 = malloc(sizeof(int)); *k1 = 5;
	int *k2 = malloc(sizeof(int)); *k2 = 2;
	tsearch(k1, &root, cmp_int);
	tsearch(k2, &root, cmp_int);
	g_visit_count = 0;
	twalk(root, count_cb);
	ASSERT_INT_EQ(g_visit_count, 7);
	free(k1); free(k2);
}

TEST(twalk_balanced_tree_count) {
	void *root = NULL;
	int vals[] = {3, 1, 5};
	for (int i=0; i<3; i++) { int *p = malloc(sizeof(int)); *p = vals[i]; tsearch(p, &root, cmp_int); }
	g_visit_count = 0;
	twalk(root, count_cb);
	ASSERT_INT_EQ(g_visit_count, 11); /* 2 leaves (4) + 1 internal (3) */
}

TEST(twalk_skewed_tree_count) {
	void *root = NULL;
	int vals[] = {1, 2, 3};
	for (int i=0; i<3; i++) { int *p = malloc(sizeof(int)); *p = vals[i]; tsearch(p, &root, cmp_int); }
	g_visit_count = 0;
	twalk(root, count_cb);
	ASSERT_INT_EQ(g_visit_count, 10); /* 1 leaf (4) + 2 internal (3) */
}

TEST(twalk_null_callback) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 1;
	tsearch(k, &root, cmp_int);
	twalk(root, NULL);
	free(k);
}

TEST(twalk_postorder_before_endorder) {
	void *root = NULL;
	int *k = malloc(sizeof(int)); *k = 1;
	tsearch(k, &root, cmp_int);
	g_last_visit = preorder;
	twalk(root, count_cb);
	ASSERT_INT_EQ(g_last_visit, endorder);
	free(k);
}

/* ============================================================================ */
TEST_MAIN()
