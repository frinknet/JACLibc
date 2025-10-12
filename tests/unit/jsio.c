/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <jsio.h>

TEST_TYPE(unit);
TEST_UNIT(jsio.h);

/* ============================================================================
 * NODE CREATION AND LIFECYCLE TESTS
 * ============================================================================ */
TEST_SUITE(node_lifecycle);

TEST(create_null_node) {
	js_t* node = JS_NULL;
	ASSERT_NOT_NULL(node);
	ASSERT_EQ(JS_TYPE_NULL, node->type);
	js_delete(node);
}

TEST(create_string_node) {
	js_t* node = JS_STRING("Hello World");
	ASSERT_NOT_NULL(node);
	ASSERT_EQ(JS_TYPE_STRING, node->type);
	ASSERT_STR_EQ("Hello World", node->value.str);
	ASSERT_EQ(11, js_length(node));
	js_delete(node);
}

TEST(create_number_node) {
	js_t* node = JS_NUMBER(42.5);
	ASSERT_NOT_NULL(node);
	ASSERT_EQ(JS_TYPE_NUMBER, node->type);
	ASSERT_FLOAT_EQ(42.5, node->value.num, 0.001);
	js_delete(node);
}

TEST(create_boolean_node) {
	js_t* true_node = JS_BOOLEAN(true);
	js_t* false_node = JS_BOOLEAN(false);
	
	ASSERT_NOT_NULL(true_node);
	ASSERT_EQ(JS_TYPE_BOOLEAN, true_node->type);
	ASSERT_FLOAT_EQ(1.0, true_node->value.num, 0.001);
	
	ASSERT_NOT_NULL(false_node);
	ASSERT_EQ(JS_TYPE_BOOLEAN, false_node->type);
	ASSERT_FLOAT_EQ(0.0, false_node->value.num, 0.001);
	
	js_delete(true_node);
	js_delete(false_node);
}

TEST(create_array_node) {
	js_t* arr = JS_ARRAY;
	ASSERT_NOT_NULL(arr);
	ASSERT_EQ(JS_TYPE_ARRAY, arr->type);
	ASSERT_EQ(0, js_length(arr));
	ASSERT_NULL(arr->first);
	js_delete(arr);
}

TEST(create_object_node) {
	js_t* obj = JS_OBJECT;
	ASSERT_NOT_NULL(obj);
	ASSERT_EQ(JS_TYPE_OBJECT, obj->type);
	ASSERT_EQ(0, js_length(obj));
	ASSERT_NULL(obj->first);
	js_delete(obj);
}

TEST(delete_node_cleanup) {
	js_t* node = JS_STRING("test");
	js_delete(node);
	// Should not crash
}

/* ============================================================================
 * TREE ATTACHMENT AND DETACHMENT TESTS
 * ============================================================================ */
TEST_SUITE(tree_operations);

TEST(attach_child_to_array) {
	js_t* arr = JS_ARRAY;
	js_t* child = JS_NUMBER(10);
	
	js_attach(child, arr);
	
	ASSERT_EQ(arr, child->parent);
	ASSERT_EQ(child, arr->first);
	ASSERT_EQ(1, js_length(arr));
	
	js_delete(arr);
}

TEST(attach_multiple_children) {
	js_t* arr = JS_ARRAY;
	js_t* c1 = JS_NUMBER(1);
	js_t* c2 = JS_NUMBER(2);
	js_t* c3 = JS_NUMBER(3);
	
	js_attach(c1, arr);
	js_attach(c2, arr);
	js_attach(c3, arr);
	
	ASSERT_EQ(3, js_length(arr));
	ASSERT_EQ(c1, arr->first);
	ASSERT_EQ(c2, c1->next);
	ASSERT_EQ(c3, c2->next);
	ASSERT_NULL(c3->next);
	
	js_delete(arr);
}

TEST(detach_child_from_parent) {
	js_t* arr = JS_ARRAY;
	js_t* c1 = JS_NUMBER(1);
	js_t* c2 = JS_NUMBER(2);
	
	js_attach(c1, arr);
	js_attach(c2, arr);
	
	js_detach(c1);
	
	ASSERT_NULL(c1->parent);
	ASSERT_NULL(c1->next);
	ASSERT_EQ(1, js_length(arr));
	ASSERT_EQ(c2, arr->first);
	
	js_delete(c1);
	js_delete(arr);
}

TEST(detach_middle_child) {
	js_t* arr = JS_ARRAY;
	js_t* c1 = JS_NUMBER(1);
	js_t* c2 = JS_NUMBER(2);
	js_t* c3 = JS_NUMBER(3);
	
	js_attach(c1, arr);
	js_attach(c2, arr);
	js_attach(c3, arr);
	
	js_detach(c2);
	
	ASSERT_EQ(2, js_length(arr));
	ASSERT_EQ(c3, c1->next);
	ASSERT_NULL(c2->parent);
	
	js_delete(c2);
	js_delete(arr);
}

TEST(replace_node) {
	js_t* arr = JS_ARRAY;
	js_t* old = JS_NUMBER(1);
	js_t* new = JS_NUMBER(99);
	
	js_attach(old, arr);
	js_replace(old, new);
	
	ASSERT_EQ(arr, new->parent);
	ASSERT_EQ(new, arr->first);
	ASSERT_NULL(old->parent);
	
	js_delete(arr);
}

TEST(attach_null_checks) {
	js_t* node = JS_NUMBER(5);
	js_attach(NULL, node);  // Should not crash
	js_attach(node, NULL);  // Should not crash
	js_delete(node);
}

TEST(detach_null_checks) {
	js_detach(NULL);  // Should not crash
}

/* ============================================================================
 * ARRAY OPERATION TESTS
 * ============================================================================ */
TEST_SUITE(array_operations);

TEST(push_to_array) {
	js_t* arr = JS_ARRAY;
	js_t* v1 = JS_NUMBER(1);
	js_t* v2 = JS_NUMBER(2);
	
	js_push(arr, v1);
	js_push(arr, v2);
	
	ASSERT_EQ(2, js_length(arr));
	ASSERT_EQ(v1, arr->first);
	ASSERT_EQ(v2, v1->next);
	
	js_delete(arr);
}

TEST(pop_from_array) {
	js_t* arr = JS_ARRAY;
	js_t* v1 = JS_NUMBER(1);
	js_t* v2 = JS_NUMBER(2);
	
	js_push(arr, v1);
	js_push(arr, v2);
	
	js_t* popped = js_pop(arr);
	
	ASSERT_EQ(v2, popped);
	ASSERT_EQ(1, js_length(arr));
	ASSERT_NULL(popped->parent);
	
	js_delete(popped);
	js_delete(arr);
}

TEST(pop_empty_array) {
	js_t* arr = JS_ARRAY;
	js_t* popped = js_pop(arr);
	
	ASSERT_NULL(popped);
	js_delete(arr);
}

TEST(unshift_to_array) {
	js_t* arr = JS_ARRAY;
	js_t* v1 = JS_NUMBER(1);
	js_t* v2 = JS_NUMBER(2);
	
	js_unshift(arr, v1);
	js_unshift(arr, v2);
	
	ASSERT_EQ(2, js_length(arr));
	ASSERT_EQ(v2, arr->first);
	ASSERT_EQ(v1, v2->next);
	
	js_delete(arr);
}

TEST(shift_from_array) {
	js_t* arr = JS_ARRAY;
	js_t* v1 = JS_NUMBER(1);
	js_t* v2 = JS_NUMBER(2);
	
	js_push(arr, v1);
	js_push(arr, v2);
	
	js_t* shifted = js_shift(arr);
	
	ASSERT_EQ(v1, shifted);
	ASSERT_EQ(1, js_length(arr));
	ASSERT_EQ(v2, arr->first);
	
	js_delete(shifted);
	js_delete(arr);
}

TEST(shift_empty_array) {
	js_t* arr = JS_ARRAY;
	js_t* shifted = js_shift(arr);
	
	ASSERT_NULL(shifted);
	js_delete(arr);
}

TEST(array_indexing) {
	js_t* arr = JS_ARRAY;
	js_t* v0 = JS_NUMBER(10);
	js_t* v1 = JS_NUMBER(20);
	js_t* v2 = JS_NUMBER(30);
	
	js_push(arr, v0);
	js_push(arr, v1);
	js_push(arr, v2);
	
	ASSERT_EQ(v0, js_index(arr, 0));
	ASSERT_EQ(v1, js_index(arr, 1));
	ASSERT_EQ(v2, js_index(arr, 2));
	ASSERT_NULL(js_index(arr, 3));
	ASSERT_NULL(js_index(arr, -1));
	
	js_delete(arr);
}

TEST(array_indexof) {
	js_t* arr = JS_ARRAY;
	js_t* v0 = JS_NUMBER(10);
	js_t* v1 = JS_NUMBER(20);
	
	js_push(arr, v0);
	js_push(arr, v1);
	
	ASSERT_EQ(0, js_indexof(arr, v0));
	ASSERT_EQ(1, js_indexof(arr, v1));
	
	js_t* v2 = JS_NUMBER(30);
	ASSERT_EQ(-1, js_indexof(arr, v2));
	
	js_delete(v2);
	js_delete(arr);
}

/* ============================================================================
 * OBJECT PROPERTY TESTS
 * ============================================================================ */
TEST_SUITE(object_properties);

TEST(set_and_get_property) {
	js_t* obj = JS_OBJECT;
	js_t* prop = JS_NUMBER(42);
	prop->key = strdup("answer");
	
	js_attach(prop, obj);
	
	js_t* found = js_property(obj, "answer");
	ASSERT_EQ(prop, found);
	ASSERT_FLOAT_EQ(42.0, found->value.num, 0.001);
	
	js_delete(obj);
}

TEST(property_not_found) {
	js_t* obj = JS_OBJECT;
	js_t* found = js_property(obj, "missing");
	
	ASSERT_NULL(found);
	js_delete(obj);
}

TEST(multiple_properties) {
	js_t* obj = JS_OBJECT;
	
	js_t* name = JS_STRING("Alice");
	name->key = strdup("name");
	js_attach(name, obj);
	
	js_t* age = JS_NUMBER(30);
	age->key = strdup("age");
	js_attach(age, obj);
	
	ASSERT_EQ(2, js_length(obj));
	ASSERT_STR_EQ("Alice", js_property(obj, "name")->value.str);
	ASSERT_FLOAT_EQ(30.0, js_property(obj, "age")->value.num, 0.001);
	
	js_delete(obj);
}

TEST(property_null_checks) {
	js_t* obj = JS_OBJECT;
	ASSERT_NULL(js_property(NULL, "key"));
	ASSERT_NULL(js_property(obj, NULL));
	js_delete(obj);
}

/* ============================================================================
 * VALUE MODIFICATION TESTS
 * ============================================================================ */
TEST_SUITE(value_modification);

TEST(modify_string_value) {
	js_t* str = JS_STRING("Hello");
	
	js_string(str, "World");
	
	ASSERT_STR_EQ("World", str->value.str);
	ASSERT_EQ(5, js_length(str));
	
	js_delete(str);
}

TEST(modify_number_value) {
	js_t* num = JS_NUMBER(10);
	
	js_number(num, 99.5);
	
	ASSERT_FLOAT_EQ(99.5, num->value.num, 0.001);
	
	js_delete(num);
}

TEST(modify_boolean_value) {
	js_t* b = JS_BOOLEAN(false);
	
	js_boolean(b, true);
	
	ASSERT_FLOAT_EQ(1.0, b->value.num, 0.001);
	
	js_delete(b);
}

TEST(modify_string_empty) {
	js_t* str = JS_STRING("test");
	js_string(str, "");
	
	ASSERT_STR_EQ("", str->value.str);
	ASSERT_EQ(0, js_length(str));
	
	js_delete(str);
}

TEST(modify_string_large) {
	js_t* str = JS_STRING("short");
	char large[1000];
	memset(large, 'A', 999);
	large[999] = '\0';
	
	js_string(str, large);
	
	ASSERT_EQ(999, js_length(str));
	ASSERT_EQ('A', str->value.str[0]);
	
	js_delete(str);
}

/* ============================================================================
 * JSON PARSING TESTS
 * ============================================================================ */
TEST_SUITE(json_parsing);

TEST(parse_null) {
	js_t* node = js_parse("null");
	ASSERT_NOT_NULL(node);
	ASSERT_EQ(JS_TYPE_NULL, node->type);
	js_delete(node);
}

TEST(parse_boolean_true) {
	js_t* node = js_parse("true");
	ASSERT_EQ(JS_TYPE_BOOLEAN, node->type);
	ASSERT_FLOAT_EQ(1.0, node->value.num, 0.001);
	js_delete(node);
}

TEST(parse_boolean_false) {
	js_t* node = js_parse("false");
	ASSERT_EQ(JS_TYPE_BOOLEAN, node->type);
	ASSERT_FLOAT_EQ(0.0, node->value.num, 0.001);
	js_delete(node);
}

TEST(parse_number_integer) {
	js_t* node = js_parse("42");
	ASSERT_EQ(JS_TYPE_NUMBER, node->type);
	ASSERT_FLOAT_EQ(42.0, node->value.num, 0.001);
	js_delete(node);
}

TEST(parse_number_negative) {
	js_t* node = js_parse("-123");
	ASSERT_EQ(JS_TYPE_NUMBER, node->type);
	ASSERT_FLOAT_EQ(-123.0, node->value.num, 0.001);
	js_delete(node);
}

TEST(parse_number_float) {
	js_t* node = js_parse("3.14159");
	ASSERT_EQ(JS_TYPE_NUMBER, node->type);
	ASSERT_FLOAT_EQ(3.14159, node->value.num, 0.00001);
	js_delete(node);
}

TEST(parse_string) {
	js_t* node = js_parse("\"Hello World\"");
	ASSERT_EQ(JS_TYPE_STRING, node->type);
	ASSERT_STR_EQ("Hello World", node->value.str);
	js_delete(node);
}

TEST(parse_empty_string) {
	js_t* node = js_parse("\"\"");
	ASSERT_EQ(JS_TYPE_STRING, node->type);
	ASSERT_STR_EQ("", node->value.str);
	js_delete(node);
}

TEST(parse_empty_array) {
	js_t* node = js_parse("[]");
	ASSERT_EQ(JS_TYPE_ARRAY, node->type);
	ASSERT_EQ(0, js_length(node));
	js_delete(node);
}

TEST(parse_array_numbers) {
	js_t* node = js_parse("[1,2,3]");
	ASSERT_EQ(JS_TYPE_ARRAY, node->type);
	ASSERT_EQ(3, js_length(node));
	
	ASSERT_FLOAT_EQ(1.0, js_index(node, 0)->value.num, 0.001);
	ASSERT_FLOAT_EQ(2.0, js_index(node, 1)->value.num, 0.001);
	ASSERT_FLOAT_EQ(3.0, js_index(node, 2)->value.num, 0.001);
	
	js_delete(node);
}

TEST(parse_array_mixed) {
	js_t* node = js_parse("[1,\"two\",true,null]");
	ASSERT_EQ(JS_TYPE_ARRAY, node->type);
	ASSERT_EQ(4, js_length(node));
	
	ASSERT_EQ(JS_TYPE_NUMBER, js_index(node, 0)->type);
	ASSERT_EQ(JS_TYPE_STRING, js_index(node, 1)->type);
	ASSERT_EQ(JS_TYPE_BOOLEAN, js_index(node, 2)->type);
	ASSERT_EQ(JS_TYPE_NULL, js_index(node, 3)->type);
	
	js_delete(node);
}

TEST(parse_empty_object) {
	js_t* node = js_parse("{}");
	ASSERT_EQ(JS_TYPE_OBJECT, node->type);
	ASSERT_EQ(0, js_length(node));
	js_delete(node);
}

TEST(parse_object_simple) {
	js_t* node = js_parse("{\"name\":\"Alice\",\"age\":30}");
	ASSERT_EQ(JS_TYPE_OBJECT, node->type);
	ASSERT_EQ(2, js_length(node));
	
	js_t* name = js_property(node, "name");
	ASSERT_NOT_NULL(name);
	ASSERT_STR_EQ("Alice", name->value.str);
	
	js_t* age = js_property(node, "age");
	ASSERT_NOT_NULL(age);
	ASSERT_FLOAT_EQ(30.0, age->value.num, 0.001);
	
	js_delete(node);
}

TEST(parse_nested_object) {
	js_t* node = js_parse("{\"person\":{\"name\":\"Bob\"}}");
	ASSERT_EQ(JS_TYPE_OBJECT, node->type);
	
	js_t* person = js_property(node, "person");
	ASSERT_NOT_NULL(person);
	ASSERT_EQ(JS_TYPE_OBJECT, person->type);
	
	js_t* name = js_property(person, "name");
	ASSERT_STR_EQ("Bob", name->value.str);
	
	js_delete(node);
}

TEST(parse_nested_array) {
	js_t* node = js_parse("[[1,2],[3,4]]");
	ASSERT_EQ(JS_TYPE_ARRAY, node->type);
	ASSERT_EQ(2, js_length(node));
	
	js_t* arr0 = js_index(node, 0);
	ASSERT_EQ(JS_TYPE_ARRAY, arr0->type);
	ASSERT_FLOAT_EQ(1.0, js_index(arr0, 0)->value.num, 0.001);
	
	js_delete(node);
}

/* ============================================================================
 * JSON STRINGIFY TESTS
 * ============================================================================ */
TEST_SUITE(json_stringify);

TEST(stringify_null) {
	js_t* node = JS_NULL;
	char* json = js_stringify(node);
	ASSERT_STR_EQ("null", json);
	free(json);
	js_delete(node);
}

TEST(stringify_boolean) {
	js_t* t = JS_BOOLEAN(true);
	char* json_t = js_stringify(t);
	ASSERT_STR_EQ("true", json_t);
	free(json_t);
	js_delete(t);
	
	js_t* f = JS_BOOLEAN(false);
	char* json_f = js_stringify(f);
	ASSERT_STR_EQ("false", json_f);
	free(json_f);
	js_delete(f);
}

TEST(stringify_number) {
	js_t* node = JS_NUMBER(42);
	char* json = js_stringify(node);
	ASSERT_STR_EQ("42", json);
	free(json);
	js_delete(node);
}

TEST(stringify_string) {
	js_t* node = JS_STRING("Hello");
	char* json = js_stringify(node);
	ASSERT_STR_EQ("\"Hello\"", json);
	free(json);
	js_delete(node);
}

TEST(stringify_string_with_quotes) {
	js_t* node = JS_STRING("He said \"hi\"");
	char* json = js_stringify(node);
	ASSERT_STR_EQ("\"He said \\\"hi\\\"\"", json);
	free(json);
	js_delete(node);
}

TEST(stringify_empty_array) {
	js_t* arr = JS_ARRAY;
	char* json = js_stringify(arr);
	ASSERT_STR_EQ("[]", json);
	free(json);
	js_delete(arr);
}

TEST(stringify_array) {
	js_t* arr = JS_ARRAY;
	js_push(arr, JS_NUMBER(1));
	js_push(arr, JS_NUMBER(2));
	js_push(arr, JS_NUMBER(3));
	
	char* json = js_stringify(arr);
	ASSERT_STR_EQ("[1,2,3]", json);
	free(json);
	js_delete(arr);
}

TEST(stringify_empty_object) {
	js_t* obj = JS_OBJECT;
	char* json = js_stringify(obj);
	ASSERT_STR_EQ("{}", json);
	free(json);
	js_delete(obj);
}

TEST(stringify_object) {
	js_t* obj = JS_OBJECT;
	
	js_t* name = JS_STRING("Alice");
	name->key = strdup("name");
	js_attach(name, obj);
	
	js_t* age = JS_NUMBER(30);
	age->key = strdup("age");
	js_attach(age, obj);
	
	char* json = js_stringify(obj);
	// Order may vary, just check it contains both
	ASSERT_TRUE(strstr(json, "\"name\":\"Alice\"") != NULL);
	ASSERT_TRUE(strstr(json, "\"age\":30") != NULL);
	
	free(json);
	js_delete(obj);
}

/* ============================================================================
 * ROUNDTRIP TESTS (PARSE -> STRINGIFY)
 * ============================================================================ */
TEST_SUITE(roundtrip);

TEST(roundtrip_number) {
	const char* original = "42";
	js_t* parsed = js_parse(original);
	char* stringified = js_stringify(parsed);
	
	ASSERT_STR_EQ(original, stringified);
	
	free(stringified);
	js_delete(parsed);
}

TEST(roundtrip_array) {
	const char* original = "[1,2,3]";
	js_t* parsed = js_parse(original);
	char* stringified = js_stringify(parsed);
	
	ASSERT_STR_EQ(original, stringified);
	
	free(stringified);
	js_delete(parsed);
}

TEST(roundtrip_boolean) {
	js_t* t = js_parse("true");
	char* json_t = js_stringify(t);
	ASSERT_STR_EQ("true", json_t);
	free(json_t);
	js_delete(t);
	
	js_t* f = js_parse("false");
	char* json_f = js_stringify(f);
	ASSERT_STR_EQ("false", json_f);
	free(json_f);
	js_delete(f);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(null_node_operations) {
	ASSERT_NULL(js_index(NULL, 0));
	ASSERT_EQ(-1, js_indexof(NULL, NULL));
	ASSERT_NULL(js_property(NULL, "key"));
	ASSERT_NULL(js_pop(NULL));
	ASSERT_NULL(js_shift(NULL));
}

TEST(empty_string_operations) {
	js_t* str = JS_STRING("");
	ASSERT_EQ(0, js_length(str));
	js_delete(str);
}

TEST(deeply_nested_structure) {
	js_t* root = JS_OBJECT;
	js_t* level1 = JS_OBJECT;
	level1->key = strdup("level1");
	js_attach(level1, root);
	
	js_t* level2 = JS_OBJECT;
	level2->key = strdup("level2");
	js_attach(level2, level1);
	
	js_t* value = JS_NUMBER(42);
	value->key = strdup("value");
	js_attach(value, level2);
	
	js_t* found_l1 = js_property(root, "level1");
	ASSERT_NOT_NULL(found_l1);
	
	js_t* found_l2 = js_property(found_l1, "level2");
	ASSERT_NOT_NULL(found_l2);
	
	js_t* found_val = js_property(found_l2, "value");
	ASSERT_FLOAT_EQ(42.0, found_val->value.num, 0.001);
	
	js_delete(root);
}

TEST_MAIN()
