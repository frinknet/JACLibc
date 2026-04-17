/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _JSIO_H
#define _JSIO_H
#pragma once

#include <config.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JS_MAX_DEPTH
#define JS_MAX_DEPTH 64
#endif

#if JACL_ARCH_WASM
#define JS_EXPORT(name) __attribute__((used, export_name(#name)))
#define JS_IMPORT(name) __attribute__((import_module("env"), import_name(#name))) extern
#else
#define JS_EXPORT(name)
#define JS_IMPORT(name)
#endif

//S Type enumeration
typedef enum {
	JS_TYPE_NULL = '0',
	JS_TYPE_ARRAY  = 'a',
	JS_TYPE_BOOLEAN = 'b',
	JS_TYPE_STRING	= 'c',
	JS_TYPE_NUMBER	= 'd',
	JS_TYPE_OBJECT	= 'e',
	JS_TYPE_FUNCTION	= 'f'
} jsio_type_t;

typedef struct jsio_t {
		jsio_type_t		 type;		// tag
		union {
				char*		 str;
				double	 num;				// numbers & bools
		} value;
		size_t				length;  // containers; strings via strlen
		char*					key;		 // property name if child of object
		struct jsio_t*	next;		 // next sibling
		struct jsio_t*	first;	 // first child
		struct jsio_t*	parent;  // parent
} jsio_t;

// Node Lifecycle
void js_attach(jsio_t* c, jsio_t* p);
void js_detach(jsio_t* c);
void js_replace(jsio_t* o, jsio_t* n);
jsio_t* js_create(jsio_type_t type, uint32_t klen, uint32_t vlen);
void js_delete(jsio_t* v);
void js_publish(jsio_t* n);
void js_unpublish(jsio_t* n);

// Indexing
jsio_t* js_index(jsio_t* a, int i);
int js_indexof(jsio_t* a, jsio_t* v);

// Node path
JS_EXPORT(_root) jsio_t* js_root(jsio_t* v);
char* js_path(jsio_t* v);
bool js_includes(jsio_t* r, jsio_t* v);
bool js_ispublic(jsio_t* v);

// Setters for key and value
jsio_t* js_setkey(jsio_t* x, const char* s);
jsio_t* js_string(jsio_t* x, const char* s);
jsio_t* js_number(jsio_t* x, double n);
jsio_t* js_boolean(jsio_t* x, bool n);

#define JS_NULL js_create(JS_TYPE_NULL,0,0)
#define JS_ARRAY js_create(JS_TYPE_ARRAY,0,0)
#define JS_OBJECT js_create(JS_TYPE_OBJECT,0,0)
#define JS_FUNCTION js_create(JS_TYPE_FUNCTION,0,0)
#define JS_STRING(x) js_string(js_create(JS_TYPE_STRING,0,0), (char*)(x))
#define JS_NUMBER(x) js_number(js_create(JS_TYPE_NUMBER,0,0), (double)(x))
#define JS_BOOLEAN(x) js_boolean(js_create(JS_TYPE_BOOLEAN,0,0), (bool)(x))

#if JACL_OS_JSRUN
JS_IMPORT(js)
uintptr_t	js_code(const char *code, int len, ...);
#define JS_CODE(...) js_code(#__VA_ARGS__, sizeof(#__VA_ARGS__) - 1)
#else
#define JS_CODE(...)
#define NO_JS_EXTERNALS
#endif

#ifndef NO_JS_EXTERNALS
#define JS_EXEC(fn, ...) js_code("[f,...a]=arguments;return this.import(this.export(f)(...a.map(this.export)))", 76, fn, __VA_ARGS__)
#else
#define NO_JS_NOTIFY
#define NO_JS_ASYNCIFY
#define JS_EXEC(fn, ...)
#endif

// Notify
#ifdef NO_JS_NOTIFY
static inline void js_notify(jsio_t* v) { (void*)v; }
#else
static inline void js_notify(jsio_t* v) {
	if (!js_ispublic(v)) return;

	js_code("this.trigger(this.export(arguments[0]))", 39, JS_STRING(js_path(v)));
}
#endif

// Slep and async
#ifndef NO_JS_ASYNCIFY
void js_pause();
void js_sleep(uint32_t ms);
void js_resume(void);
#endif

// Accessors
static inline char* js_key(jsio_t* v) {
	if (!v) return NULL;

	return v->key;
}
static inline size_t js_length(jsio_t* v) {
	if (!v) return 0;

	if (v->type == 'c') return strlen(v->value.str);

	return v->length;
}
static inline void* js_value(jsio_t* v) {
	if (!v) return NULL;

	switch (v->type) {
		case JS_TYPE_NUMBER:
		case JS_TYPE_BOOLEAN:
				return &v->value.num;
		case JS_TYPE_STRING:
				return &v->value.str;
		case JS_TYPE_ARRAY:
		case JS_TYPE_OBJECT:
				return &v->first;
		default:
				return NULL;
	}
}

// Array Operations
void	js_push(jsio_t* a, jsio_t* v);
jsio_t* js_pop(jsio_t* a);
void js_unshift(jsio_t* a, jsio_t* v);
jsio_t* js_shift(jsio_t* a);

// Object Property
jsio_t* js_property(jsio_t* o, const char* key);

// JS Parser
jsio_t* js_parse(const char* s);
jsio_t* js_resolve(jsio_t* root, const char* path);
char* js_stringify(jsio_t* v);

// JS Start if wasm
#if JACL_OS_JSRUN
ssize_t js_read(int fd, void* buf, size_t cnt);
ssize_t js_write(int fd, const void* buf, size_t cnt);
#endif

// JS lifecycle
JS_EXPORT(_attach) void js_attach(jsio_t* c, jsio_t* p);
JS_EXPORT(_detach) void js_detach(jsio_t* c);
JS_EXPORT(_replace) void js_replace(jsio_t* o, jsio_t* n);
JS_EXPORT(_create) jsio_t* js_create(jsio_type_t type, uint32_t klen, uint32_t vlen);
JS_EXPORT(_delete) void js_delete(jsio_t* x);

#ifdef __cplusplus
}
#endif

#endif // _JSIO_H
