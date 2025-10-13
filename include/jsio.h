/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef JSIO_H
#define JSIO_H
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
} js_type_t;

typedef struct js_t {
		js_type_t		 type;		// tag
		union {
				char*		 str;
				double	 num;				// numbers & bools
		} value;
		size_t				length;  // containers; strings via strlen
		char*					key;		 // property name if child of object
		struct js_t*	next;		 // next sibling
		struct js_t*	first;	 // first child
		struct js_t*	parent;  // parent
} js_t;

// Node Lifecycle
void js_attach(js_t* c, js_t* p);
void js_detach(js_t* c);
void js_replace(js_t* o, js_t* n);
js_t* js_create(js_type_t type, uint32_t klen, uint32_t vlen);
void js_delete(js_t* v);
void js_publish(js_t* n);
void js_unpublish(js_t* n);

// Indexing
js_t* js_index(js_t* a, int i);
int js_indexof(js_t* a, js_t* v);

// Node path
JS_EXPORT(_root) js_t* js_root(js_t* v);
char* js_path(js_t* v);
bool js_includes(js_t* r, js_t* v);
bool js_ispublic(js_t* v);

// Setters for key and value
js_t* js_string(js_t* x, const char* s);
js_t* js_number(js_t* x, double n);
js_t* js_boolean(js_t* x, bool n);

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
static inline void js_notify(js_t* v) { (void*)v; }
#else
static inline void js_notify(js_t* v) {
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
static inline char* js_key(js_t* v) {
	return v->key;
}
static inline size_t js_length(js_t* v) {
	if (!v) return 0;

	if (v->type == 'c') return strlen(v->value.str);

	return v->length;
}
static inline void* js_value(js_t* v) {
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
void	js_push(js_t* a, js_t* v);
js_t* js_pop(js_t* a);
void js_unshift(js_t* a, js_t* v);
js_t* js_shift(js_t* a);

// Object Property
js_t* js_property(js_t* o, const char* key);

// JS Parser
js_t* js_parse(const char* s);
char* js_stringify(js_t* v);

// JS Start if wasm
#if JACL_OS_JSRUN
JS_EXPORT(_start) void js_start();
ssize_t js_read(int fd, void* buf, size_t cnt);
ssize_t js_write(int fd, const void* buf, size_t cnt);
#endif

// JS lifecycle
JS_EXPORT(_attach) void js_attach(js_t* c, js_t* p);
JS_EXPORT(_detach) void js_detach(js_t* c);
JS_EXPORT(_replace) void js_replace(js_t* o, js_t* n);
JS_EXPORT(_create) js_t* js_create(js_type_t type, uint32_t klen, uint32_t vlen);
JS_EXPORT(_delete) void js_delete(js_t* x);

#ifdef __cplusplus
}
#endif

#endif // JSIO_H
