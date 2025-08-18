// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef JSIO_H
#define JSIO_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JS_MAX_DEPTH
#define JS_MAX_DEPTH 64
#endif

#define JS_EXPORT(name) __attribute__((used, export_name(name)))
#define JS_IMPORT(name) __attribute__((import_module("env"), import_name(name))) extern
#define JS_START static JS_EXPORT(_start)
#define JS_CODE(code, ...) js_code(#code, sizeof(#code) - 1, __VA_ARGS__)

JS_IMPORT(js) double	js_code(const char *code, int len, ...);

//S Type enumeration
typedef enum {
	JS_TYPE_NULL = '0',
	JS_TYPE_ARR  = 'a',
	JS_TYPE_BOOL = 'b',
	JS_TYPE_STR  = 'c',
	JS_TYPE_NUM  = 'd',
	JS_TYPE_OBJ  = 'e',
	JS_TYPE_FNC  = 'f'
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

JS_EXPORT(data)
js_t* JS_ROOT;

// Create & constructors
JS_EXPORT(nodeCreate)
js_t* js_create(char type, uint32_t klen, uint32_t vlen) {
	size_t len = sizeof(js_t) + klen + (type == JS_TYPE_STR ? vlen + 1 : 0);
	js_t* x	= malloc(len);

	memset(x, 0, sizeof(js_t));
	x->type	 = type;

	return x;
}
#define JS_NULL js_create(JS_TYPE_NULL)
#define JS_FROM(x) _Generic((x), \
	js_t:					x \
	bool:					js_set_num(js_create(JS_TYPE_BOOL), x), \
	int:					js_set_num(js_create(JS_TYPE_NUM), x), \
	double:				js_set_num(js_create(JS_TYPE_NUM), x), \
	float:				js_set_num(js_create(JS_TYPE_NUM), x), \
	char*					js_set_str(js_create(JS_TYPE_STR), x), \
	const char*		js_set_str(js_create(JS_TYPE_STR), x), \
	default:			JS_NULL, \
)(x)

// Memory management
JS_EXPORT(nodeDestroy)
static inline void js_free(js_t* x) {
	if (!x) return;

	js_detach(x);

	for (js_t* c = x->first, *t; c; c = t) {
		t = c->next;

		js_free(c);
	}

	free(x->key);

	if (x->type == JS_TYPE_STR) free(x->value.str);

	free(x);
}

// Attach/detach (arrays & objects)
JS_EXPORT(nodeAttach)
static inline void js_attach(js_t* c, js_t* p) {
	if (!c || !p) return;

	c->parent = p;

	if (!p->first) {
		p->first = c;
	} else {
		js_t* q = p->first;

		while (q->next) q = q->next;

		q->next = c;
	}

	p->length++;

	js_notify(c);
}
JS_EXPORT(nodeDetatch)
static inline void js_detach(js_t* c) {
	if (!c || !c->parent) return;

	js_t** link = c->parent->first;

	while (*link && *link != c) link = &(*link)->next;

	if (*link) {
		*link = c->next;

		c->parent->length--;
	}

	link = c->parent;

	c->parent = c->next = NULL;

	js_notify(link);
}
JS_EXPORT(nodeReplace)
static inline void js_replace(js_t* o, js_t* n) {
	if (!o || !n) return;

	// Transfer linkage
	n->next = o->next;
	n->parent = o->parent;

	// If we're at the head of parent's children
	if (n->parent && n->parent->first == o) {
			n->parent->first = n;
	} else if (n->parent) {
			js_t *p = n->parent->first;

			while (p && p->next != o) p = p->next;

			if (p) p->next = n;
	}

	// Remove old node from list
	o->next = NULL;
	o->parent = NULL;

	js_notify(n);
	js_free(o);
}

// Setters for key and value
static inline js_t* js_set_key(js_t* x, const char* key) {
	if (!x || !key) return x;

	size_t klen = strlen(key);
	char* nk = realloc(x->key, klen + 1);

	if (!nk) return x;						// OOM: keep old key

	memcpy(nk, key, klen + 1);

	x->key = nk;

	js_notify(x);

	return x;
}
static inline js_t* js_set_str(js_t* x, const char* s) {
	if (!x || x->type != JS_TYPE_STR || !s) return x;

	size_t slen = strlen(s);
	char* sptr = realloc(x->value.str, slen + 1);

	if (sptr) {
		memcpy(sptr, s, slen + 1);

		x->value.str = sptr;
		x->length = slen;

		js_notify(x);
	}

	return x;
}
static inline js_t* js_set_num(js_t* x, double n) {
	if (!x || (x->type != JS_TYPE_NUM && x->type != JS_TYPE_BOOL)) return x;

	x->value.num = n;
	x->length = 1;

	js_notify(x);

	return x;
}

// length
static inline size_t js_length(js_t* v) {
	if (!v) return 0;

	return &v->length;
}


#ifndef JS_NO_EXTERNALS
JS_CODE({
const
A={
has:(o,k)=>!!S(o,k),
get:(o,k)=>X(S(o,k)),
getPrototypeOf:o=>('a'===RT(o)?Array:Object).prototype,
deleteProperty:(o,k)=>!this.exports.nodeDestroy(S(o,k)),
set:(o,k,v)=>!!W(o,k,v),
D=(x=>m =>x.decode(m))(new TextDecoder),
E=(x=>m =>x.encode(m))(new TextEncoder),
F=[],
H=[],
I=(a,i)=>{ for(let p=Y(a),c=0;p&&c<i;p=Z(p),++c);return p},
L=p=>R32(p+16),
M=new DataView(this.exports.memory.buffer),
P=(o,k)=>{for(let p=Y(o);p;p=Z(p))if(S(RK(p))===k)return p},
S=(o,k)=>'a'===RT(o)?I(o,k):P(o,k),
T=p=>String.fromCharCode(R8(p)),
V=v=>v===null?'0':Array.isArray(v)?'a':{boolean:'b',string:'c',number:'d',object:'e',function:'f'}[typeof v],
W=(p,k,v)=>{
const
o=p?S(p,k):0,
t=V(v),
n=this.exports.nodeCreate(t,k.length,v.length);
if(p && k)WK(x,k);
if(t==='c')WC(x,v);
else if(t==='d')WD(x,v);
else if(t==='b')WB(x,v);
else if(t==='f')WF(x,v);
else for(let x in v)W(n,t==='e'?x:0,v[x]); 
o?this.exports.nodeReplace(o,n):this.exports.nodeAttach(n,p);
},
X=p=>{switch(T(p)){
case'0':return null;
case'a':return RG(p);
case'b':return RB(p);
case'c':return RC(p);
case'd':return RD(p);
case'e':return RG(p);
case'f':return RF(p);
default:return undefined;
}},
Y=p=>R32(p+28),
Z=p=>R32(p+24),
R8=p=>M.getUint8(p),
W8=(p,v)=>M.setUint8(p,v)),
R32=p=>M.getUint32(p),
W32=(p,v)=>M.setUint32(p,v)),
R64=p=>M.getFloat64(p),
W64=(p,v)=>M.getFloat64(p),
RS=p=>D(Uint8Array.from(function*(i,b){for(; (b=R8(i)); ++i) yield b}(p))),
WS=(p,s)=>[...E(s), 0].forEach((b, i) => W8(p + i, b)),
RK=p=>RS(R32(p+20)),
WK=(p,v)=>WS(R32(p+20)),
RB=p=>!!RD(p),
WB=(p,v)=>WD(p,!!v),
RC=p=>RS(R32(p+8)),
WC=p=>WS(R32(p+8)),
RD=p=>R64(p+8),
WD=(p,v)=>W64(p+8,v),
RF=p=>F[RD(p)],
WF=(p,v)=>WD(p, F.includes(v)?F.indexOf(v):F.push(v)-1);
RG=p=>new Proxy(p,A),
this.data=X(this.exports.data);
this.export=X;
this.import=v=>W(0,0,v);
this.listen=(pt,fn)=>H.push([new RegExp(pt.replace(/\./g,'\\.').replace(/\*/g,'.*')),fn]);
this.trigger=async pt=>H.forEach(async([re,fn])=>re.test(path)&&fn(path));
});
#define JS_NO_NOTIFY 
#define JS_NO_ASYNC 
#define JS_EXEC(fn, ...) JS_CODE("cost [f,...a]=arguments;return this.import(this.export(f)(...a.map(this.export)))", __VA_ARGS__)
#else
#define JS_EXEC(fn, ...)
#endif


// Slep and async
#ifndef JS_NO_ASYNC
_t* js_async = JS_CODE({
let SP = 0,SM;
const SH = 64*1024,SO = {
pause:sp=>{SP=sp;SM=new Uint8Array(this.exports.memory.buffer,SP,SH);throw 'PAUSE'},
sleep:ms=>setTimeout(SO.resume,ms),
resume:()=>{new Uint8Array(this.exports.memory.buffer,SP,SH).set(SM);this.exports.resume();}
};
return this.import(SO);
});
#define JS_PAUSE JS_EXEC(js_property(js_async, "pause"), (uint32_t)(uintptr_t)__builtin_frame_address(0));
#define JS_SLEEP JS_PAUSE; JS_EXEC(js_property(js_async, "sleep"), ms);
#define JS_RESUME js_resume
JS_EXPORT(pause)
void js_pause() { JS_PAUSE; }
JS_EXPORT(sleep)
void js_sleep(uint32_t ms) { JS_SLEEP(ms); }
JS_EXPORT(resume)
void js_resume(void) {}
#endif

// Node path
char* js_path(js_t* v) {
	if (!v) return NULL;

	// Collect segments
	char* segments[JS_MAX_DEPTH];
	int seg_count = 0;
	js_t* cur = v;

	while (cur && seg_count < JS_MAX_DEPTH) {
		char buf[64];

		if (cur->parent && cur->parent->type == JS_TYPE_ARR) {
			// find index in parent array
			int i = 0;

			js_t* sib = cur->parent->first;

			while (sib && sib != cur) { sib = sib->next; i++; }

			snprintf(buf, sizeof(buf), "[%d]", i);
		} else if (cur->key) {
			snprintf(buf, sizeof(buf), ".%s", cur->key);
		} else {
			snprintf(buf, sizeof(buf), "$");
		}

		segments[seg_count++] = strdup(buf);
		cur = cur->parent;
	}

	// Estimate length
	size_t total = 1;

	// Build full path
	for (int i = seg_count - 1; i >= 0; i--) total += strlen(segments[i]);

	char* path = malloc(total);

	path[0] = '\0';

	for (int i = seg_count - 1; i >= 0; i--) {

		strcat(path, segments[i]);
		free(segments[i]);
	}

	return path;
}

// In root
static inline bool js_inroot(js_t* v) {
	for (js_t* cur = v; cur; cur = cur->parent)
		if (cur == JS_ROOT) return true;

	return false;
}

#ifndef JS_NO_NOTIFY
void js_notify(js_t* v) {
	if (!is_inroot(v)) return;

	JS_CODE("this.trigger(this.export(arguments[0]))", JS_FROM(js_path(v)));
}
#else
void js_notify(js_t* v) {}
#endif

// Universal value pointer accessor
static inline void* js_value(js_t* v) {
	if (!v) return NULL;

	switch (v->type) {
		case JS_TYPE_NUM:
		case JS_TYPE_BOOL:
				return &v->value.num;
		case JS_TYPE_STR:
				return &v->value.str;
		case JS_TYPE_ARR:
		case JS_TYPE_OBJ:
				return &v->first;
		default:
				return NULL;
	}
}

// Array operations
static inline void	js_push(js_t* a, js_t* v) {
	if (a && a->type == JS_TYPE_ARR) js_attach(v, a);
}
static inline js_t* js_pop(js_t* a) {
	if (!a || a->type != JS_TYPE_ARR || !a->first) return NULL;

	js_t* p = NULL, *c = a->first;

	while (c->next) { p = c; c = c->next; }

	if (p) p->next = NULL; else a->first = NULL;

	c->parent = c->next = NULL;
	a->length--;

	js_notify(a);

	return c;
}
static inline void js_unshift(js_t* a, js_t* v) {
		if (a && a->type == JS_TYPE_ARR) {
				v->next		= a->first;
				v->parent = a;
				a->first	= v;
				a->length++;

				js_notify(v);
		}
}
static inline js_t* js_shift(js_t* a) {
		if (!a || a->type != JS_TYPE_ARR || !a->first) return NULL;
		js_t* c			= a->first;
		a->first		= c->next;
		c->parent		= c->next = NULL;
		a->length--;

		js_notify(a);

		return c;
}

// Indexing
static inline js_t* js_index(js_t* a, int i) {
	if (!a || a->type != JS_TYPE_ARR || i < 0) return NULL;

	js_t* c = a->first;

	for (int j = 0; j < i && c; j++) c = c->next;

	return c;
}
static inline int js_indexof(js_t* a, js_t* v) {
	if (!a || a->type != JS_TYPE_ARR || !v) return -1;

	js_t* c = a->first;
	int i = 0;

	while (c) {
		if (c == v) return i;

		c = c->next;
		i++;
	}

	return -1;
}

// Property access
static inline js_t* js_property(js_t* o, const char* key) {
	if (!o || !key || o->type != JS_TYPE_OBJ) return NULL;

	for (js_t* c = o->first; c; c = c->next)
		if (c->key && strcmp(c->key, key) == 0) return c;

	return NULL;
}

// JS Parser
static js_t* js_parse_value(const char* s, size_t* i);
static js_t* js_parse_object(const char* s, size_t* i) {
	js_t* o = js_create(JS_TYPE_OBJ); (*i)++; // skip '{'

	while (s[*i] && s[*i] != '}') {
		while (isspace((unsigned char)s[*i]) || s[*i]=='"') (*i)++;

		size_t start = *i;

		while (s[*i] && s[*i] != '"') (*i)++;

		char* key = strndup(s + start, (*i)++ - start);

		while (s[*i] != ':') (*i)++; (*i)++;

		js_t* v = js_parse_value(s, i);
		v->key = key; js_attach(v, o);

		if (s[*i] == ',') (*i)++;
	}

	(*i)++; // skip '}'

	return o;
}
static js_t* js_parse_array(const char* s, size_t* i) {
	js_t* a = js_create(JS_TYPE_ARR); (*i)++; // skip '['

	while (s[*i] && s[*i] != ']') {
		js_t* v = js_parse_value(s, i);
		js_attach(v, a);

		if (s[*i] == ',') (*i)++;
	}

	(*i)++; // skip ']'

	return a;
}
static js_t* js_parse_string(const char* s, size_t* i) {
	(*i)++; size_t start = *i;

	while (s[*i] && s[*i] != '"') (*i)++;

	js_t* str = JS_FROM(strndup(s + start, (*i)++ - start));

	return str;
}
static js_t* js_parse_number(const char* s, size_t* i) {
	char* end;
	double n = strtod(s + *i, &end);
	js_t* num = JS_FROM(n);
	*i = end - s;

	return num;
}
static js_t* js_parse_value(const char* s, size_t* i) {
	while (isspace((unsigned char)s[*i])) (*i)++;
	if (s[*i] == '{')	return js_parse_object(s, i);
	if (s[*i] == '[')	return js_parse_array(s, i);
	if (s[*i] == '"')	return js_parse_string(s, i);
	if (s[*i] == '-' || isdigit((unsigned char)s[*i])) return js_parse_number(s, i);
	if (!strncmp(s + *i, "true", 4))	{ *i += 4; return JS_FROM(1); }
	if (!strncmp(s + *i, "false",5))	{ *i += 5; return JS_FROM(0); }
	if (!strncmp(s + *i, "null", 4))	{ *i += 4; return JS_NULL; }

	return JS_NULL;
}
js_t* js_parse(const char* s) {
	size_t i = 0;

	return js_parse_value(s, &i);
}
char* js_strigify(js_t* v) {
	if (!v) return strdup("null");
	
	switch (v->type) {
	case JS_TYPE_NULL:
		return strdup("null");
	case JS_TYPE_BOOL:
		return strdup(v->value.num ? "true" : "false");
	case JS_TYPE_NUM: {
		char* buf = malloc(32);
		snprintf(buf, 32, "%.17g", v->value.num);
		return buf;
	}
	case JS_TYPE_STR: {
		size_t len = strlen(v->value.str);
		char* buf = malloc(len * 2 + 3); // worst case: all chars escaped + quotes + null
		char* p = buf;
		*p++ = '"';
		for (char* s = v->value.str; *s; s++) {
			if (*s == '"' || *s == '\\') {
				*p++ = '\\';
				*p++ = *s;
			} else if (*s == '\n') {
				*p++ = '\\';
				*p++ = 'n';
			} else {
				*p++ = *s;
			}
		}
		*p++ = '"';
		*p = '\0';
		return buf;
	}
	case JS_TYPE_ARR: {
		size_t cap = 64, pos = 0;
		char* buf = malloc(cap);
		buf[pos++] = '[';
		
		for (js_t* c = v->first; c; c = c->next) {
			if (c != v->first) {
				if (pos >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
				buf[pos++] = ',';
			}
			char* child = js_tojson(c);
			size_t child_len = strlen(child);
			while (pos + child_len >= cap) { cap *= 2; buf = realloc(buf, cap); }
			strcpy(buf + pos, child);
			pos += child_len;
			free(child);
		}
		
		if (pos >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
		buf[pos++] = ']';
		buf[pos] = '\0';
		return buf;
	}
	case JS_TYPE_OBJ: {
		size_t cap = 64, pos = 0;
		char* buf = malloc(cap);
		buf[pos++] = '{';
		
		for (js_t* c = v->first; c; c = c->next) {
			if (c != v->first) {
				if (pos >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
				buf[pos++] = ',';
			}
			
			// Add key
			size_t key_len = strlen(c->key);
			while (pos + key_len + 3 >= cap) { cap *= 2; buf = realloc(buf, cap); }
			buf[pos++] = '"';
			strcpy(buf + pos, c->key);
			pos += key_len;
			buf[pos++] = '"';
			buf[pos++] = ':';
			
			// Add value
			char* child = js_tojson(c);
			size_t child_len = strlen(child);
			while (pos + child_len >= cap) { cap *= 2; buf = realloc(buf, cap); }
			strcpy(buf + pos, child);
			pos += child_len;
			free(child);
		}
		
		if (pos >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
		buf[pos++] = '}';
		buf[pos] = '\0';
		return buf;
	}
	default:
		return strdup("null");
	}
}

#ifdef __cplusplus
}
#endif

#endif // JSIO_H
