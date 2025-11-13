/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <jsio.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(jsio.h);

/* ============================================================================
 * SUITE: js_create / js_delete
 * ============================================================================ */
TEST_SUITE(js_create_js_delete);

TEST(js_create_basic) {
	js_t* n = js_create(JS_TYPE_NUMBER, 0, 0);
	ASSERT_EQ(JS_TYPE_NUMBER, n->type);
	js_delete(n);
}

TEST(js_create_with_lengths) {
	js_t* s = js_create(JS_TYPE_STRING, 10, 50);
	ASSERT_EQ(JS_TYPE_STRING, s->type);
	js_delete(s);
}

TEST(js_delete_null) {
	js_delete(NULL); // Should not crash
}

TEST(js_delete_tree) {
	js_t* arr = JS_ARRAY;
	for (int i = 0; i < 8; ++i) js_attach(JS_NUMBER(i), arr);
	js_delete(arr);
}

/* ============================================================================
 * SUITE: js_attach
 * ============================================================================ */
TEST_SUITE(js_attach);

TEST(js_attach_nulls) {
	js_attach(NULL, NULL);
}

TEST(js_attach_only_child) {
	js_t* c = JS_NUMBER(7);
	js_attach(c, NULL);
	ASSERT_NULL(c->parent);
	js_delete(c);
}

TEST(js_attach_only_parent) {
	js_t* arr = JS_ARRAY;
	js_attach(NULL, arr);
	ASSERT_EQ(0, js_length(arr));
	js_delete(arr);
}

TEST(js_attach_self) {
	js_t* n = JS_OBJECT;
	js_attach(n, n);
	ASSERT_TRUE(n->parent == n || n->parent == NULL);
	js_delete(n);
}

TEST(js_attach_chain) {
	js_t* arr = JS_ARRAY;
	js_t* n1 = JS_NUMBER(1); js_attach(n1, arr);
	js_t* n2 = JS_NUMBER(2); js_attach(n2, arr);
	js_t* n3 = JS_NUMBER(3); js_attach(n3, arr);
	ASSERT_EQ(3, js_length(arr));
	ASSERT_EQ(n1, arr->first);
	ASSERT_EQ(n2, n1->next);
	ASSERT_EQ(n3, n2->next);
	ASSERT_NULL(n3->next);
	js_delete(arr);
}

TEST(js_attach_same_child_twice) {
	js_t* arr = JS_ARRAY;
	js_t* n = JS_NUMBER(1);
	js_attach(n, arr);
	js_attach(n, arr);
	ASSERT_EQ(arr, n->parent);
	js_delete(arr);
}

/* ============================================================================
 * SUITE: js_detach
 * ============================================================================ */
TEST_SUITE(js_detach);

TEST(js_detach_null) {
	js_detach(NULL);
}

TEST(js_detach_head_and_tail) {
	js_t* arr = JS_ARRAY;
	js_t* n1 = JS_NUMBER(101); js_attach(n1, arr);
	js_t* n2 = JS_NUMBER(202); js_attach(n2, arr);
	js_detach(n1);
	ASSERT_EQ(n2, arr->first);
	js_detach(n2);
	ASSERT_EQ(0, js_length(arr));
	js_delete(n1); js_delete(n2); js_delete(arr);
}

TEST(js_detach_middle_child) {
	js_t* arr = JS_ARRAY;
	js_t* n1 = JS_NUMBER(1); js_attach(n1, arr);
	js_t* n2 = JS_NUMBER(2); js_attach(n2, arr);
	js_t* n3 = JS_NUMBER(3); js_attach(n3, arr);
	js_detach(n2);
	ASSERT_EQ(n3, n1->next);
	ASSERT_NULL(n2->parent);
	js_delete(n2); js_delete(arr);
}

TEST(js_detach_orphan) {
	js_t* n = JS_BOOLEAN(true);
	js_detach(n);
	ASSERT_NULL(n->parent);
	js_delete(n);
}

/* ============================================================================
 * SUITE: js_replace
 * ============================================================================ */
TEST_SUITE(js_replace);

TEST(js_replace_nulls) {
	js_replace(NULL, NULL);
}

TEST(js_replace_head) {
	js_t* arr = JS_ARRAY;
	js_t* old = JS_NUMBER(11); js_attach(old, arr);
	js_t* rep = JS_NUMBER(99);
	js_replace(old, rep);
	ASSERT_EQ(rep, arr->first);
	ASSERT_NULL(old->parent);
	js_delete(arr);
}

TEST(js_replace_mid) {
	js_t* arr = JS_ARRAY;
	js_t* n1 = JS_NUMBER(1); js_attach(n1, arr);
	js_t* old = JS_NUMBER(2); js_attach(old, arr);
	js_t* n3 = JS_NUMBER(3); js_attach(n3, arr);
	js_t* rep = JS_NUMBER(8);
	js_replace(old, rep);
	ASSERT_EQ(rep, n1->next);
	ASSERT_EQ(n3, rep->next);
	ASSERT_NULL(old->parent);
	js_delete(arr);
}

/* ============================================================================
 * SUITE: js_index
 * ============================================================================ */
TEST_SUITE(js_index);

TEST(js_index_happy) {
	js_t* arr = JS_ARRAY;
	js_t* a = JS_NUMBER(10); js_attach(a, arr);
	js_t* b = JS_NUMBER(20); js_attach(b, arr);
	ASSERT_EQ(a, js_index(arr, 0));
	ASSERT_EQ(b, js_index(arr, 1));
	ASSERT_NULL(js_index(arr, 2));
	ASSERT_NULL(js_index(NULL, 0));
	js_delete(arr);
}

TEST(js_index_negative) {
	js_t* arr = JS_ARRAY;
	ASSERT_NULL(js_index(arr, -1));
	js_delete(arr);
}

TEST(js_indexof_found_and_missing) {
	js_t* arr = JS_ARRAY;
	js_t* x = JS_NUMBER(42); js_attach(x, arr);
	ASSERT_EQ(0, js_indexof(arr, x));
	ASSERT_EQ(-1, js_indexof(arr, NULL));
	ASSERT_EQ(-1, js_indexof(NULL, x));
	js_delete(arr);
	js_delete(x);
}

/* ============================================================================
 * SUITE: js_length
 * ============================================================================ */
TEST_SUITE(js_length);

TEST(js_length_null) {
	ASSERT_EQ(0, js_length(NULL));
}

TEST(js_length_string) {
	js_t* s = JS_STRING("abcd");
	ASSERT_EQ(4, js_length(s));
	js_delete(s);
}

TEST(js_length_array_object) {
	js_t* a = JS_ARRAY;
	js_t* o = JS_OBJECT;
	js_attach(JS_NUMBER(29), a);
	js_attach(JS_STRING("k"), o);
	ASSERT_EQ(1, js_length(a));
	ASSERT_EQ(1, js_length(o));
	js_delete(a);
	js_delete(o);
}

/* ============================================================================
 * SUITE: js_value
 * ============================================================================ */
TEST_SUITE(js_value);

TEST(js_value_null) {
	ASSERT_NULL(js_value(NULL));
}

TEST(js_value_number_ptr) {
	js_t* n = JS_NUMBER(8.88);
	ASSERT_NOT_NULL(js_value(n));
	ASSERT_FLT_EQ(8.88, *(double*)js_value(n));
	js_delete(n);
}

TEST(js_value_string_ptr) {
	js_t* s = JS_STRING("xyz");
	ASSERT_NOT_NULL(js_value(s));
	ASSERT_STR_EQ("xyz", *(char**)js_value(s));
	js_delete(s);
}

TEST(js_value_array_object_ptr) {
	js_t* a = JS_ARRAY;
	js_t* o = JS_OBJECT;
	ASSERT_NOT_NULL(js_value(a));
	ASSERT_NOT_NULL(js_value(o));
	js_delete(a);
	js_delete(o);
}

/* ============================================================================
 * SUITE: js_property
 * ============================================================================ */
TEST_SUITE(js_property);

TEST(js_property_find) {
	js_t* obj = JS_OBJECT;
	js_t* foo = JS_NUMBER(12);
	foo->key = malloc(8); strcpy(foo->key, "foo"); // If you have a proper key setter, use it!
	js_attach(foo, obj);
	ASSERT_EQ(foo, js_property(obj, "foo"));
	js_delete(obj);
}

TEST(js_property_missing) {
	js_t* obj = JS_OBJECT;
	ASSERT_NULL(js_property(obj, "notfound"));
	js_delete(obj);
}

TEST(js_property_null_args) {
	ASSERT_NULL(js_property(NULL, "key"));
	ASSERT_NULL(js_property(JS_OBJECT, NULL));
}

/* ============================================================================
 * SUITE: js_parse / js_stringify
 * ============================================================================ */
TEST_SUITE(js_parse);

TEST(js_parse_edge_cases) {
	js_t* nul = js_parse("null");
	ASSERT_NOT_NULL(nul); ASSERT_EQ(JS_TYPE_NULL, nul->type);
	js_delete(nul);

	js_t* t = js_parse("true");
	ASSERT_EQ(JS_TYPE_BOOLEAN, t->type);
	ASSERT_FLT_EQ(1.0, t->value.num);
	js_delete(t);

	js_t* f = js_parse("false");
	ASSERT_EQ(JS_TYPE_BOOLEAN, f->type);
	ASSERT_FLT_EQ(0.0, f->value.num);
	js_delete(f);

	js_t* num = js_parse("3.14159");
	ASSERT_EQ(JS_TYPE_NUMBER, num->type);
	ASSERT_FLT_EQ(3.14159, num->value.num);
	js_delete(num);

	js_t* str = js_parse("\"hi\"");
	ASSERT_EQ(JS_TYPE_STRING, str->type);
	ASSERT_STR_EQ("hi", str->value.str);
	js_delete(str);
}

TEST(js_parse_arrays_and_objs) {
	js_t* arr = js_parse("[1,\"a\",true,null]");
	ASSERT_EQ(JS_TYPE_ARRAY, arr->type);
	ASSERT_EQ(4, js_length(arr));
	ASSERT_EQ(JS_TYPE_NUMBER, js_index(arr, 0)->type);
	ASSERT_EQ(JS_TYPE_STRING, js_index(arr, 1)->type);
	ASSERT_EQ(JS_TYPE_BOOLEAN, js_index(arr, 2)->type);
	ASSERT_EQ(JS_TYPE_NULL, js_index(arr, 3)->type);
	js_delete(arr);

	js_t* obj = js_parse("{\"k\":17}");
	ASSERT_EQ(JS_TYPE_OBJECT, obj->type);
	ASSERT_EQ(1, js_length(obj));
	ASSERT_EQ(JS_TYPE_NUMBER, js_property(obj, "k")->type);
	ASSERT_FLT_EQ(17.0, js_property(obj, "k")->value.num);
	js_delete(obj);
}

TEST(js_parse_nested_structs) {
	js_t* node = js_parse("{\"outer\":[{\"x\":1},null]}");
	ASSERT_EQ(JS_TYPE_OBJECT, node->type);
	js_t* arr = js_property(node, "outer");
	ASSERT_EQ(JS_TYPE_ARRAY, arr->type);
	ASSERT_EQ(2, js_length(arr));
	ASSERT_EQ(JS_TYPE_OBJECT, js_index(arr,0)->type);
	ASSERT_EQ(JS_TYPE_NULL, js_index(arr,1)->type);
	js_delete(node);
}

/* ============================================================================
 * SUITE: js_stringify
 * ============================================================================ */
TEST_SUITE(js_stringify);

TEST(js_stringify_scalar) {
	js_t* n = JS_NUMBER(3);
	char* out = js_stringify(n);
	ASSERT_STR_EQ("3", out);
	free(out); js_delete(n);
}

TEST(js_stringify_array_and_obj) {
	js_t* arr = JS_ARRAY;
	js_push(arr, JS_NUMBER(1));
	js_push(arr, JS_STRING("yo"));
	char* out = js_stringify(arr);
	ASSERT_TRUE(strstr(out, "1") && strstr(out, "\"yo\""));
	free(out);
	js_delete(arr);
}

TEST(js_stringify_object_orderless) {
	js_t* obj = JS_OBJECT;
	js_t* prop1 = JS_STRING("value");
	prop1->key = malloc(8); strcpy(prop1->key, "key");
	js_attach(prop1, obj);

	js_t* prop2 = JS_NUMBER(2);
	prop2->key = malloc(8); strcpy(prop2->key, "other");
	js_attach(prop2, obj);

	char* j = js_stringify(obj);
	ASSERT_TRUE(strstr(j,"key") && strstr(j,"other"));
	free(j); js_delete(obj);
}

/* ============================================================================
 * SUITE: Accessors (js_key/js_length/js_value)
 * ============================================================================ */
TEST_SUITE(js_key);

TEST(js_key_null) {
	ASSERT_NULL(js_key(NULL));
}

TEST(js_key_direct_assign) {
	js_t* s = JS_STRING("a");
	s->key = malloc(16); strcpy(s->key, "special");
	ASSERT_STR_EQ("special", js_key(s));
	js_delete(s);
}

/* ============================================================================
 * SUITE: Roundtrip (parse -> stringify)
 * ============================================================================ */
TEST_SUITE(js_roundtrip);

TEST(js_roundtrip_number) {
	const char* json = "42";
	js_t* node = js_parse(json);
	char* out = js_stringify(node);
	ASSERT_STR_EQ(json, out);
	free(out); js_delete(node);
}

TEST(js_roundtrip_array) {
	const char* json = "[1,2,3]";
	js_t* node = js_parse(json);
	char* out = js_stringify(node);
	ASSERT_STR_EQ(json, out);
	free(out); js_delete(node);
}

/* ============================================================================
 * SUITE: Edge Cases
 * ============================================================================ */
TEST_SUITE(js_edge_cases);

TEST(js_null_node_operations) {
	ASSERT_NULL(js_index(NULL, 0));
	ASSERT_EQ(-1, js_indexof(NULL, NULL));
	ASSERT_NULL(js_property(NULL, "key"));
	ASSERT_NULL(js_pop(NULL));
	ASSERT_NULL(js_shift(NULL));
}

TEST(js_empty_string_ops) {
	js_t* s = JS_STRING("");
	ASSERT_EQ(0, js_length(s));
	js_delete(s);
}

TEST(js_deep_nested) {
	js_t* r = JS_OBJECT;
	js_t* l1 = JS_OBJECT; l1->key = malloc(16); strcpy(l1->key, "l1"); js_attach(l1, r);
	js_t* l2 = JS_OBJECT; l2->key = malloc(16); strcpy(l2->key, "l2"); js_attach(l2, l1);
	js_t* val = JS_NUMBER(123); val->key = malloc(16); strcpy(val->key, "val"); js_attach(val, l2);
	ASSERT_FLT_EQ(123.0, js_property(js_property(js_property(r,"l1"),"l2"),"val")->value.num);
	js_delete(r);
}

/* ============================================================================
 * SUITE: Setters (js_string, js_number, js_boolean)
 * ============================================================================ */
TEST_SUITE(js_setters);

TEST(js_string_setter) {
	js_t* node = JS_STRING("old");
	js_string(node, "new");
	ASSERT_STR_EQ("new", node->value.str);
	ASSERT_EQ(3, js_length(node));
	js_delete(node);
}

TEST(js_number_setter) {
	js_t* node = JS_NUMBER(1);
	js_number(node, 6.28);
	ASSERT_FLT_EQ(6.28, node->value.num);
	js_delete(node);
}

TEST(js_boolean_setter) {
	js_t* node = JS_BOOLEAN(0);
	js_boolean(node, true);
	ASSERT_FLT_EQ(1.0, node->value.num);
	js_delete(node);
}

TEST_MAIN()

