/* (c) 2025 FRINKnet & Friends - MIT licence */
#ifndef CORE_JSIO_H
#define CORE_JSIO_H

#include <jsio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JS_PUBLIC_ROOT __jacl_public_root()
static jsio_t *__jacl_public_root(void) {
	static jsio_t* root = NULL;
	return root;
}

// Node Lifecycle
void js_attach(jsio_t* c, jsio_t* p) {
	if (!c || !p) return;

	c->parent = p;

	if (!p->first) {
		p->first = c;
	} else {
		jsio_t* q = p->first;

		while (q->next) q = q->next;

		q->next = c;
	}

	p->length++;

	js_notify(c);
}
void js_detach(jsio_t* c) {
	if (!c || !c->parent) return;

	jsio_t* prev = NULL;
	jsio_t* curr = c->parent->first;

	while (curr && curr != c) {
		prev = curr;
		curr = curr->next;
	}

	if (curr == c) {
		if (prev) prev->next	 = c->next;
		else		 c->parent->first = c->next;
		c->parent->length--;
	}

	c->parent = c->next = NULL;

	js_notify(curr ? (prev? prev : c->parent) : c->parent);
}
void js_replace(jsio_t* o, jsio_t* n) {
	if (!o || !n) return;

	// Transfer linkage
	n->next = o->next;
	n->parent = o->parent;

	// If we're at the head of parent's children
	if (n->parent && n->parent->first == o) {
			n->parent->first = n;
	} else if (n->parent) {
			jsio_t *p = n->parent->first;

			while (p && p->next != o) p = p->next;

			if (p) p->next = n;
	}

	// Remove old node from list
	o->next = NULL;
	o->parent = NULL;

	js_notify(n);
	js_delete(o);
}
jsio_t* js_create(jsio_type_t type, uint32_t klen, uint32_t vlen) {
	size_t len = sizeof(jsio_t) + klen + (type == JS_TYPE_STRING ? vlen + 1 : 0);
	jsio_t* x	= (jsio_t*)malloc(len);

	memset(x, 0, sizeof(jsio_t));
	x->type	 = type;

	return x;
}
void js_delete(jsio_t* x) {
	if (!x || x == JS_PUBLIC_ROOT) return;

	js_detach(x);

	for (jsio_t* c = x->first, *t; c; c = t) {
		t = c->next;

		js_delete(c);
	}

	free(x->key);

	if (x->type == JS_TYPE_STRING) free(x->value.str);

	free(x);
}
void js_publish(jsio_t* n) {
	if(js_includes(JS_PUBLIC_ROOT, n)) return;

	js_attach(n, JS_PUBLIC_ROOT);
}
void js_unpublish(jsio_t* n) {
	if(!js_includes(JS_PUBLIC_ROOT, n)) return;

	js_detach(n);
}

// Indexing
jsio_t* js_index(jsio_t* a, int i) {
	if (!a || a->type != JS_TYPE_ARRAY || i < 0) return NULL;

	jsio_t* c = a->first;

	for (int j = 0; j < i && c; j++) c = c->next;

	return c;
}
int js_indexof(jsio_t* a, jsio_t* v) {
	if (!a || a->type != JS_TYPE_ARRAY || !v) return -1;

	jsio_t* c = a->first;
	int i = 0;

	while (c) {
		if (c == v) return i;

		c = c->next;
		i++;
	}

	return -1;
}

// Node
jsio_t* js_root(jsio_t* v) {
	jsio_t* cur = v;

	while (cur->parent) cur = cur->parent;

	return cur;
}
char* js_path(jsio_t* v) {
		if (!v) return NULL;

		char* segments[JS_MAX_DEPTH]; int n=0;

		for (jsio_t* cur=v; cur&&n<JS_MAX_DEPTH; cur=cur->parent) {
				char buf[64] = {0};

				if (cur->parent && cur->parent->type==JS_TYPE_ARRAY) snprintf(buf,sizeof(buf),"[%d]", js_indexof(cur->parent,cur));
				else if (cur->key) snprintf(buf,sizeof(buf),".%s",cur->key);

				segments[n++] = strdup(buf);
		}

		size_t total=1; for(int i=n-1;i>=0;--i) total+=strlen(segments[i]);
		char* path = (char*)malloc(total);

		path[0]=0;

		for(int i=n-1;i>=0;--i){ strcat(path,segments[i]); free(segments[i]); }
		if (path[0]=='.') memmove(path,path+1,strlen(path));

		return path;
}
bool js_includes(jsio_t* r, jsio_t* v) {
	for (jsio_t* cur = v; cur; cur = cur->parent)
		if (cur == r) return true;

	return false;
}
bool js_ispublic(jsio_t* v) {
	return js_includes(JS_PUBLIC_ROOT, v);
}

// Setters for key and value
char* js_setkey(jsio_t* x, const char* key) {
	if (!x || !key) return x->key;

	size_t klen = strlen(key);
	char* nk = (char*)realloc(x->key, klen + 1);

	memcpy(nk, key, klen + 1);

	x->key = nk;

	js_notify(x);

	return x->key;
}
jsio_t* js_string(jsio_t* x, const char* s) {
	if (!x || x->type != JS_TYPE_STRING || !s) return x;

	size_t slen = strlen(s);
	char* sptr = (char*)realloc(x->value.str, slen + 1);

	if (sptr) {
		memcpy(sptr, s, slen + 1);

		x->value.str = sptr;
		x->length = slen;

		js_notify(x);
	}

	return x;
}
jsio_t* js_number(jsio_t* x, double n) {
	if (!x || (x->type != JS_TYPE_NUMBER && x->type != JS_TYPE_BOOLEAN)) return x;

	x->value.num = n;
	x->length = 1;

	js_notify(x);

	return x;
}
jsio_t* js_boolean(jsio_t* x, bool b) {
	return js_number(x, (double)b);
}

// Array operations
void	js_push(jsio_t* a, jsio_t* v) { if (a && a->type == JS_TYPE_ARRAY) js_attach(v, a); }
jsio_t* js_pop(jsio_t* a) {
	if (!a || a->type != JS_TYPE_ARRAY || !a->first) return NULL;

	jsio_t* p = NULL, *c = a->first;

	while (c->next) { p = c; c = c->next; }

	if (p) p->next = NULL; else a->first = NULL;

	c->parent = c->next = NULL;
	a->length--;

	js_notify(a);

	return c;
}
void js_unshift(jsio_t* a, jsio_t* v) {
		if (a && a->type == JS_TYPE_ARRAY) {
				v->next		= a->first;
				v->parent = a;
				a->first	= v;
				a->length++;

				js_notify(v);
		}
}
jsio_t* js_shift(jsio_t* a) {
		if (!a || a->type != JS_TYPE_ARRAY || !a->first) return NULL;
		jsio_t* c			= a->first;
		a->first		= c->next;
		c->parent		= c->next = NULL;
		a->length--;

		js_notify(a);

		return c;
}

// Object Property
jsio_t* js_property(jsio_t* o, const char* key) {
	if (!o || !key || o->type != JS_TYPE_OBJECT) return NULL;

	for (jsio_t* c = o->first; c; c = c->next)
		if (c->key && strcmp(c->key, key) == 0) return c;

	return NULL;
}

#if JACL_ARCH_WASM

#if JACL_OS_JSRUN
static jsio_t *__jacl_js_async(jsio_t* set) {
	static jsio_t* init = NULL;

	if (set) init = set;

	return init;
}
#define JS_ASYNC __jacl_js_async()
#else
#define NO_JS_ASYNCIFY
#endif

// Declare both possible signatures as weak symbols
extern int main(void) __attribute__((weak));
extern int main(int argc, char *argv[]) __attribute__((weak));

void js_start(){
	#ifndef NO_JS_IO
	jsio_t* __jacl_js_init = JS_CODE(
		const
		//accessor
		A={
			has:(o,k)=>!!S(o,k),
			get:(o,k)=>X(S(o,k)),
			getPrototypeOf:o=>('a'===T(o)?Array:Object).prototype,
			deleteProperty:(o,k)=>!G._delete(S(o,k)),
			set:(o,k,v)=>!!W(o,k,v)
		},
		//bound
		B=this.exports,
		//decode
		D=(x=>m =>x.decode(m))(new TextDecoder),
		//encode
		E=(x=>m =>x.encode(m))(new TextEncoder),
		//functions
		F=[],
		//guarded
		G=Object.keys(B).filter(x=>x[0]==='_').reduce((o,f)=>{
			o[f]=B[f].bind(B);
			delete B[f];
			return o;
		}, {}),
		//handlers
		H=[],
		//iterate
		I=(a,i)=>{for(let p=Y(a),c=0;p&&c<i;p=Z(p),++c);return p},
		//juicing
		J={1:[,console.log],2:[,console.error]},
		//length
		L=p=>R32(p+16),
		//memory
		M=new DataView(B.memory.buffer),
		//property
		P=(o,k)=>{for(let p=Y(o);p;p=Z(p))if(S(RK(p))===k)return p},
		//search
		S=(o,k)=>'a'===T(o)?I(o,k):P(o,k),
		//type
		T=p=>String.fromCharCode(R8(p)),
		//valuetype
		V=v=>v===null?'0':Array.isArray(v)?'a':{boolean:'b',string:'c',number:'d',object:'e',function:'f'}[typeof v],
		//write
		W=(p,k,v)=>{
			const
			o=p?S(p,k):0,
			t=V(v),
			n=G._create(t,k.length,v.length);
			if(p && k)WK(n,k);
			if(t==='c')WC(n,v);
			else if(t==='d')WD(n,v);
			else if(t==='b')WB(n,v);
			else if(t==='f')WF(n,v);
			else for(let x in v)W(n,t==='e'?x:0,v[x]);
			o?G._replace(o,n):G._attach(n,p);
		},
		//extract
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
		//first
		Y=p=>R32(p+28),
		//next
		Z=p=>R32(p+24),

		//read/write
		R8=p=>M.getUint8(p),
		W8=(p,v)=>M.setUint8(p,v),
		R32=p=>M.getUint32(p),
		W32=(p,v)=>M.setUint32(p,v),
		R64=p=>M.getFloat64(p),
	 	W64=(p,v)=>M.setFloat64(p,v),

		//string
		RS=p=>D(Uint8Array.from(function*(i,b){for(; (b=R8(i)); ++i) yield b}(p))),
		WS=(p,s)=>[...E(s),0].forEach((b,i)=>W8(p+i,b)),
		//key
		RK=p=>RS(R32(p+20)),
		WK=(p,v)=>WS(R32(p+20),v),
		//bool
		RB=p=>!!RD(p),
		WB=(p,v)=>WD(p,!!v),
		//char
		RC=p=>RS(R32(p+8)),
		WC=(p,v)=>WS(R32(p+8),v),
		//digits
		RD=p=>R64(p+8),
		WD=(p,v)=>W64(p+8,v),
		//functions
		RF=p=>F[RD(p)],
		WF=(p,v)=>WD(p,F.includes(v)?F.indexOf(v):F.push(v)-1),
		//gathered
		RG=p=>new Proxy(p,A);

		this.pipe=(p,r,w)=>r?(J[p]=[r,w]):J[p];
		this.data=X(G._root());
		this.export=X;
		this.import=v=>W(0,0,v);
		this.listen=(pt,fn)=>H.push([new RegExp(pt),fn]);
		this.trigger=async pt=>H.forEach(async([re,fn])=>re.test(pt)&&fn(pt));
	);
	#endif

	#ifndef NO_JS_ASYNCIFY
	__jacl_js_async((jsio_t*)JS_CODE(
		let SP = 0,SM;
		const SH = 64*1024,SO = {
			pause:sp=>{SP=sp;SM=new Uint8Array(this.exports.memory.buffer,SP,SH);throw 'PAUSE'},
			sleep:ms=>setTimeout(SO.resume,ms),
			resume:()=>{new Uint8Array(this.exports.memory.buffer,SP,SH).set(SM);this.exports.resume();}
		};

		return this.import(SO);
	));
	#endif

	//call main
	main();
}

#ifndef JS_MAX_FDS
#define JS_MAX_FDS 64
#endif

typedef struct {
  char buffer[BUFSIZ];
  size_t pos;
  int mode;
} fd_buf_t;

static fd_buf_t fd_bufs[JS_MAX_FDS] = {0};
static bool fd_inits[JS_MAX_FDS] = {0};

static ssize_t js_read(int fd, void *buf, size_t count) {
  if (fd < 0 || fd >= JS_MAX_FDS || !buf) {
    errno = EBADF;

    return -1;
  }

	if (count == 0) return 0;

  jsio_t* result = js_code("this.import(this.pipe(arguments[0])[0](arguments[1]))", 53, fd, count);

  if (!result || result->type != JS_TYPE_STRING) return 0;

  size_t copy_len = (result->length < count) ? result->length : count;

  memcpy(buf, result->value.str, copy_len);

  return copy_len;
}

static ssize_t js_write(int fd, const void *buf, size_t count) {
  if (fd < 0 || fd >= JS_MAX_FDS || !buf) {
    errno = EBADF;
    return -1;
  }

  if (count == 0) return 0;

  // Initialize on first use
  if (!fd_inits[fd]) {
    fd_bufs[fd].mode = (fd == STDERR_FILENO)
      ? _IONBF
      : (fd <= STDERR_FILENO)
        ? _IOLBF
        : _IOFBF;
    fd_inits[fd] = true;
  }

  fd_buf_t* fdb = &fd_bufs[fd];
  const char* data = (const char*)buf;

  for (size_t i = 0; i < count; i++) {
    char c = data[i];

    // Append first
    fdb->buffer[fdb->pos++] = c;

    // Then decide to flush
    if ((fdb->mode == _IONBF) || (fdb->mode == _IOLBF && c == '\n') || (fdb->pos >= BUFSIZ - 1)) {
      fdb->buffer[fdb->pos] = '\0';

      js_code("this.pipe(arguments[0])[1](this.export(arguments[1]))", 58, fd, JS_STRING(fdb->buffer));

      fdb->pos = 0;
    }
  }

  return count;
}
#endif /* JACL_ARCH_WASM */

// Slep and async
#ifndef NO_JS_ASYNCIFY
#define JS_PAUSE JS_EXEC(js_property(JS_ASYNC, "pause"), (uint32_t)(uintptr_t)__builtin_frame_address(0));
JS_EXPORT(sleep) void js_sleep(uint32_t ms) {
	JS_PAUSE;
	JS_EXEC(js_property(JS_ASYNC, "sleep"), ms);
}
JS_EXPORT(pause) void js_pause() { JS_PAUSE; }
JS_EXPORT(resume) void js_resume(void) {  }
#undef JS_PAUSE
#endif /* !NO_JS_ASYNCIFY */

// JS Parser
static jsio_t* js_parse_value(const char* s, size_t* i);
static jsio_t* js_parse_object(const char* s, size_t* i) {
	jsio_t* o = js_create(JS_TYPE_OBJECT,0,0);
	size_t l = strlen(s);

	if (!s || !i || *i >= l || s[*i] != '{') return JS_NULL;

	(*i)++; // skip '{'

	while (*i < l && s[*i] && s[*i] != '}') {
		while (*i < l && (isspace(s[*i]) || s[*i]=='"')) (*i)++;

		if (*i >= l || s[*i] == '}') break;  // BOUNDS CHECK!

		size_t start = *i;
		while (*i < l && s[*i] != '"') (*i)++;  // BOUNDS CHECK!

		if (*i >= l) return o;  // EARLY EXIT

		char* key = strndup(s + start, (*i)++ - start);

		while (*i < l && s[*i] != ':') (*i)++;

		if (*i >= l || s[*i] != ':') { free(key); return o; }

		(*i)++;  // skip ':'
		jsio_t* v = js_parse_value(s, i);
		v->key = key;

		js_attach(v, o);

		if (*i < l && s[*i] == ',') (*i)++;
	}

	if (*i < l) (*i)++; // skip '}'

	return o;
}
static jsio_t* js_parse_array(const char* s, size_t* i) {
	jsio_t* a = js_create(JS_TYPE_ARRAY,0,0); (*i)++; // skip '['

	while (s[*i] && s[*i] != ']') {
		jsio_t* v = js_parse_value(s, i);
		js_attach(v, a);

		if (s[*i] == ',') (*i)++;
	}

	(*i)++; // skip ']'

	return a;
}
static jsio_t* js_parse_string(const char* s, size_t* i) {
	(*i)++; size_t start = *i;

	while (s[*i] && s[*i] != '"') (*i)++;

	jsio_t* str = JS_STRING(strndup(s + start, (*i)++ - start));

	return str;
}
static jsio_t* js_parse_number(const char* s, size_t* i) {
	char* end;
	double n = strtod(s + *i, &end);
	jsio_t* num = JS_NUMBER(n);
	*i = end - s;

	return num;
}
static jsio_t* js_parse_value(const char* s, size_t* i) {
	while (isspace((unsigned char)s[*i])) (*i)++;
	if (s[*i] == '{')	return js_parse_object(s, i);
	if (s[*i] == '[')	return js_parse_array(s, i);
	if (s[*i] == '"')	return js_parse_string(s, i);
	if (s[*i] == '-' || isdigit((unsigned char)s[*i])) return js_parse_number(s, i);
	if (!strncmp(s + *i, "true", 4))	{ *i += 4; return JS_BOOLEAN(1); }
	if (!strncmp(s + *i, "false",5))	{ *i += 5; return JS_BOOLEAN(0); }
	if (!strncmp(s + *i, "null", 4))	{ *i += 4; return JS_NULL; }

	return JS_NULL;
}
jsio_t* js_parse(const char* s) {
	size_t i = 0;

	return js_parse_value(s, &i);
}
jsio_t* js_resolve(jsio_t* root, const char* path) {
	if (!root || !path) return NULL;
	if (*path == '.') path++;

	jsio_t* cur = root;

	while (*path && cur) {
		if (*path == '[') {
			int idx = atoi(path + 1);

			cur = js_index(cur, idx);
			path = strchr(path, ']');

			if (path) path++;
		} else {
			while (*path == '.') path++;

			if (!*path) break;

			char key[64];
			const char* end = strpbrk(path, ".[");
			size_t len = end ? (size_t)(end - path) : strlen(path);

			if (len >= sizeof(key)) len = sizeof(key) - 1;

			strncpy(key, path, len);

			key[len] = 0;

			cur = js_property(cur, key);

			if (!end) break;

			path = end;
		}
	}

	return cur;
}
char* js_stringify(jsio_t* v) {
	if (!v) return strdup("null");

	switch (v->type) {
		case JS_TYPE_NULL: return strdup("null");
		case JS_TYPE_BOOLEAN: return strdup(v->value.num ? "true" : "false");
		case JS_TYPE_NUMBER: {
			char* buf = (char*)malloc(32);

			snprintf(buf, 32, "%.17g", v->value.num);

			return buf;
		}
		case JS_TYPE_STRING: {
			size_t len = strlen(v->value.str);
			char* buf = (char*)malloc(len * 2 + 3); // worst case: all chars escaped + quotes + null
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
		case JS_TYPE_ARRAY: {
			size_t cap = 64, pos = 0;
			char* buf = (char*)malloc(cap);

			buf[pos++] = '[';

			for (jsio_t* c = v->first; c; c = c->next) {
				if (c != v->first) {
					if (pos >= cap - 1) { cap *= 2; buf = (char*)realloc(buf, cap); }

					buf[pos++] = ',';
				}

				char* child = js_stringify(c);
				size_t child_len = strlen(child);

				while (pos + child_len >= cap) {
					cap *= 2;
					buf = (char*)realloc(buf, cap);
				}

				strcpy(buf + pos, child);

				pos += child_len;

				free(child);
			}

			if (pos >= cap - 1) { cap *= 2; buf = (char*)realloc(buf, cap); }

			buf[pos++] = ']';
			buf[pos] = '\0';

			return buf;
		}
		case JS_TYPE_OBJECT: {
			size_t cap = 64, pos = 0;
			char* buf = (char*)malloc(cap);

			buf[pos++] = '{';

			for (jsio_t* c = v->first; c; c = c->next) {
				if (c != v->first) {
					if (pos >= cap - 1) { cap *= 2; buf = (char*)realloc(buf, cap); }

					buf[pos++] = ',';
				}

				// Add key
				size_t key_len = strlen(c->key);

				while (pos + key_len + 3 >= cap) {
					cap *= 2;
					buf = (char*)realloc(buf, cap);
				}

				buf[pos++] = '"';
				strcpy(buf + pos, c->key);
				pos += key_len;
				buf[pos++] = '"';
				buf[pos++] = ':';

				// Add value
				char* child = js_stringify(c);
				size_t child_len = strlen(child);

				while (pos + child_len >= cap) {
					cap *= 2;
					buf = (char*)realloc(buf, cap);
				}

				strcpy(buf + pos, child);
				pos += child_len;
				free(child);
			}

			if (pos >= cap - 1) {
				cap *= 2;
				buf = (char*)realloc(buf, cap);
			}

			buf[pos++] = '}';
			buf[pos] = '\0';

			return buf;
		}
		default: return strdup("null");
	}
}
#ifdef __cplusplus
}
#endif

#endif /* CORE_JSIO_H */
