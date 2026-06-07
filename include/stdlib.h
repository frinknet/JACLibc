/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _STDLIB_H
#define _STDLIB_H
#pragma once

#include <config.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdbit.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#if JACL_HAS_C23
#define __STDC_VERSION_STDLIB_H__ 202311L
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Standard exit codes */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Multibyte character constants */
#define MB_CUR_MAX 1

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 1
#endif

#ifndef TMP_MAX
#define TMP_MAX 238328
#endif

/* ============================================================= */
/* Type Definitions                                              */
/* ============================================================= */

typedef struct { int8_t quot, rem; } div8_t;
typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

#if JACL_HAS_C99
	typedef struct { long long quot, rem; } lldiv_t;
#endif

/* ============================================================= */
/* Memory Allocation                                             */
/* ============================================================= */
void* malloc(size_t n);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
int posix_memalign(void** memptr, size_t alignment, size_t size);
void* reallocarray(void* ptr, size_t nmemb, size_t size);

#if JACL_HAS_C11
void *aligned_alloc(size_t a, size_t s);
#endif

/* ============================================================= */
/* Integer Conversion & Parsing                                  */
/* ============================================================= */
int atoi(const char* str);
double atof(const char* str);
long atol(const char* str);
long strtol(const char* str, char ** endptr, int base);
double strtod(const char *nptr, char **endptr);

#if JACL_HAS_C99
long long atoll(const char*);
unsigned long strtoul(const char* str, char** endptr, int base);
long long strtoll(const char*, char ** str, int base);
unsigned long long strtoull(const char* str, char** endptr, int base);
float strtof(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);
#endif /* JACL_HAS_C99 */

/* ============================================================= */
/* Absolute Value & Division                                     */
/* ============================================================= */
static inline int abs(int n) { return n < 0 ? -n : n; }
static inline long labs(long n) { return n < 0 ? -n : n; }
static inline div_t div(int n, int d) {
	if (d == 0) return (__errno_set(EDOM), (div_t){0, 0});
	return (n == INT_MIN && d == -1) ? (div_t){ INT_MIN, 0 } : (div_t){ n/d, n%d };
}
static inline ldiv_t ldiv(long n, long d) {
	if (d == 0) return (__errno_set(EDOM), (ldiv_t){0, 0});
	else return (n == LONG_MIN && d == -1) ? (ldiv_t){ LONG_MIN, 0 } : (ldiv_t){ n/d, n%d };
}

#if JACL_HAS_C99
static inline long long llabs(long long n) { return n < 0 ? -n : n; }
static inline lldiv_t lldiv(long long n, long long d) {
	if (d == 0) return (__errno_set(EDOM), (lldiv_t){0, 0});
	else return (n == LLONG_MIN && d == -1) ? (lldiv_t){ LLONG_MIN, 0 } : (lldiv_t){ n/d, n%d };
}
#endif

/* ============================================================= */
/* Pseudo-Random Number Generation                               */
/* ============================================================= */
#define RAND_MAX 0x7FFF  // Standard value (32767)

extern unsigned __jacl_rand_seed;

/* 48-bit RNG state */
extern unsigned short __jacl_rand48_seed[3];
extern unsigned short __jacl_rand48_mult[3];
extern unsigned short __jacl_rand48_add;
extern unsigned short __jacl_rand48_oldseed[3];

/* BSD random state */
extern uint_least32_t *__jacl_random_state;
extern uint_least32_t *__jacl_random_fptr;
extern uint_least32_t *__jacl_random_rptr;

static inline void srand(unsigned s) { __jacl_rand_seed = s ? s : 1; }
static inline int rand(void) { __jacl_rand_seed = __jacl_rand_seed * 1103515245u + 12345u; return (int)((__jacl_rand_seed >> 16) & RAND_MAX); }

/* ============================================================= */
/* Sorting & Searching                                           */
/* ============================================================= */

static inline void __jacl_sort_swap(unsigned char* a, unsigned char* b, size_t size) {
	while (size--) {
		unsigned char tmp = *a;

		*a++ = *b;
		*b++ = tmp;
	}
}
static inline int __jacl_sort_call(const void *a, const void *b, void *fn) { return ((int (*)(const void *, const void *))fn)(a, b); }
static inline void __jacl_sort_again(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void *arg) {
	if (nmemb < 2) return;

	unsigned char* arr = (unsigned char*)base;
	size_t low = 0, high = nmemb - 1, mid = low + (high - low) / 2;

	if (compar(arr + low * size, arr + mid * size, arg) > 0) __jacl_sort_swap(arr + low * size, arr + mid * size, size);
	if (compar(arr + mid * size, arr + high * size, arg) > 0) __jacl_sort_swap(arr + mid * size, arr + high * size, size);
	if (compar(arr + low * size, arr + mid * size, arg) > 0) __jacl_sort_swap(arr + low * size, arr + mid * size, size);

	void* pivot = malloc(size);

	if (!pivot) {
		for (size_t i = 0; i < nmemb - 1; i++) {
			for (size_t j = 0; j < nmemb - i - 1; j++) {
				if (compar(arr + j * size, arr + (j + 1) * size, arg) > 0) __jacl_sort_swap(arr + j * size, arr + (j + 1) * size, size);
			}
		}

		return;
	}

	memcpy(pivot, arr + mid * size, size);

	size_t i = 0, j = nmemb - 1;

	while (1) {
		while (i < nmemb && compar(arr + i * size, pivot, arg) < 0) i++;
		while (j > 0 && compar(arr + j * size, pivot, arg) > 0) j--;

		if (i >= j) break;

		__jacl_sort_swap(arr + i * size, arr + j * size, size);

		i++;

		if (j > 0) j--;
	}

	free(pivot);

	if (j > 0) __jacl_sort_again(arr, j + 1, size, compar, arg);
	if (i < nmemb - 1) __jacl_sort_again(arr + (i + 1) * size, nmemb - i - 1, size, compar, arg);
}

static inline void qsort_r(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void *arg) {
	if (!base || !compar || size == 0 || nmemb < 2) return;

	__jacl_sort_again(base, nmemb, size, compar, arg);
}
static inline void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	if (!base || !compar || size == 0 || nmemb < 2) return;

	__jacl_sort_again(base, nmemb, size, __jacl_sort_call, (void *)compar);
}

static inline void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	if (!key || !base || !compar || size == 0 || nmemb == 0) return NULL;

	const unsigned char* arr = (const unsigned char*)base;
	size_t low = 0, high = nmemb;

	while (low < high) {
		size_t mid = low + (high - low) / 2;
		int cmp = compar(key, arr + mid * size);

		if (cmp == 0) return (void*)(arr + mid * size);
		else if (cmp < 0) high = mid;
		else low = mid + 1;
	}

	return NULL;
}

/* ============================================================= */
/* Program Termination & Exit                                    */
/* ============================================================= */
noreturn void abort(void);
noreturn void exit(int status);
int atexit(void (*func)(void));

#if JACL_HAS_C11
noreturn void quick_exit(int status);
int at_quick_exit(void (*func)(void));
#endif

#if JACL_HAS_C99
noreturn void _Exit(int status);
#endif

/* ============================================================= */
/* Environment & System                                          */
/* ============================================================= */
static inline int system(const char* command) {
	#if JACL_OS_WINDOWS
		return _spawnlp(_P_WAIT, "cmd.exe", "cmd.exe", "/C", command, NULL);
	#elif JACL_OS_JSRUN || JACL_OS_WASM
		(void)command;
		return -1;
	#else
		if (!command) return 1;

		pid_t pid = fork();
		if (pid < 0) return -1;
		if (pid == 0) {
			execl("/bin/sh", "sh", "-c", command, (char*)NULL);
			_exit(127);
		}

		int status;
		while (waitpid(pid, &status, 0) < 0) {
			if (!__errno_chk(EINTR)) return -1;
		}

		if (WIFEXITED(status)) return WEXITSTATUS(status);
		if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);

		return -1;
	#endif
}
#if JACL_HAS_POSIX
static inline int putenv(char *string) {
	extern char **environ;

	if (!string || !strchr(string, '=')) { return (__errno_set(EINVAL), -1); }

	const char *eq = strchr(string, '=');
	size_t namelen = eq - string;

	for (char **ep = environ; *ep; ep++) {
		if (strncmp(*ep, string, namelen) == 0 && (*ep)[namelen] == '=') {
			*ep = string;

			return 0;
		}
	}

	size_t count = 0;

	for (char **ep = environ; *ep; ep++) count++;

	char **new_environ = (char**)malloc((count + 2) * sizeof(char*));

	if (!new_environ) return (__errno_set(ENOMEM), -1);

	for (size_t i = 0; i < count; i++) new_environ[i] = environ[i];

	new_environ[count] = string;
	new_environ[count + 1] = NULL;

	environ = new_environ;

	return 0;
}
static inline int setenv(const char *name, const char *value, int overwrite) {
	if (!name || !*name || strchr(name, '=')) return (__errno_set(EINVAL), -1);
	if (!value) return (__errno_set(EINVAL), -1);
	if (!overwrite && getenv(name)) return 0;

	size_t nlen = strlen(name);
	size_t vlen = strlen(value);
	size_t len = nlen + vlen + 2; // name + = + value + \0
	char *str = (char*)malloc(len);

	if (!str) return (__errno_set(ENOMEM), -1);

	// Manual construction: name=value\0
	memcpy(str, name, nlen);

	str[nlen] = '=';

	memcpy(str + nlen + 1, value, vlen);

	str[len - 1] = '\0';

	return putenv(str);
}
static inline int unsetenv(const char *name) {
	extern char **environ;

	if (!name || !*name || strchr(name, '=')) { return (__errno_set(EINVAL), -1); }

	size_t len = strlen(name);
	char **ep = environ;

	while (*ep) {
		if (strncmp(*ep, name, len) == 0 && (*ep)[len] == '=') {
			char **sp = ep;
			do { *sp = *(sp + 1); } while (*sp++);
		} else {
			ep++;
		}
	}

	return 0;
}
static inline char *realpath(const char *restrict path, char *restrict resolved_path) {
	if (!path) return (__errno_set(EINVAL), NULL);

	size_t plen = strlen(path);
	if (plen >= PATH_MAX) return (__errno_set(ENAMETOOLONG), NULL);

	char buf[PATH_MAX];
	char temp[PATH_MAX];

	if (*path == '/') {
		strcpy(buf, path);
	} else {
		if (getcwd(buf, PATH_MAX) == NULL) return NULL;

		size_t len = strlen(buf);
		if (len + plen + 1 >= PATH_MAX) return (__errno_set(ENAMETOOLONG), NULL);

		buf[len] = '/';
		strcpy(buf + len + 1, path);
	}

	for (int count = 0; count < 40; count++) {
		struct stat st;
		if (lstat(buf, &st) != 0) return NULL;

		if (!S_ISLNK(st.st_mode)) break;

		ssize_t link_len = readlink(buf, temp, PATH_MAX - 1);
		if (link_len < 0) return NULL;

		temp[link_len] = '\0';

		if (temp[0] == '/') {
			strcpy(buf, temp);
		} else {
			char *last_slash = strrchr(buf, '/');

			if (last_slash) {
				*(last_slash + 1) = '\0';
				if (strlen(buf) + link_len >= PATH_MAX) return (__errno_set(ENAMETOOLONG), NULL);
				strcat(buf, temp);
			} else {
				strcpy(buf, temp);
			}
		}
	}

	struct stat st;
	if (stat(buf, &st) != 0) return NULL;

	if (resolved_path) {
		strcpy(resolved_path, buf);
		return resolved_path;
	}

	return strdup(buf);
}
#endif

/* ============================================================= */
/* Temporary Files (POSIX)                                       */
/* ============================================================= */

#if JACL_HAS_POSIX

static inline int mkstemp(char *template) {
	if (!template) return (__errno_set(EINVAL), -1);

	size_t len = strlen(template);

	if (len < 6) return (__errno_set(EINVAL), -1);

	char *suffix = template + len - 6;
	if (strcmp(suffix, "XXXXXX") != 0) return (__errno_set(EINVAL), -1);

	static unsigned int counter = 0;
	unsigned int attempts = 0;

	while (attempts++ < TMP_MAX) {
		unsigned int seed = (unsigned int)time(NULL)
		                  ^ (unsigned int)getpid()
		                  ^ __jacl_rand_seed
		                  ^ __sync_fetch_and_add(&counter, 1);

		for (int i = 0; i < 6; i++) {
			seed = seed * 1103515245u + 12345u;
			int r = (seed >> 16) % 62;
			if (r < 10)
				suffix[i] = '0' + r;
			else if (r < 36)
				suffix[i] = 'A' + (r - 10);
			else
				suffix[i] = 'a' + (r - 36);
		}

		int fd = open(template, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) return fd;

		if (!__errno_chk(EEXIST)) return -1;
	}

	return (__errno_set(EEXIST), -1);
}

static inline char *mkdtemp(char *template) {
	if (!template) return (__errno_set(EINVAL), NULL);

	size_t len = strlen(template);

	if (len < 6) return (__errno_set(EINVAL), NULL);

	char *suffix = template + len - 6;

	if (strcmp(suffix, "XXXXXX") != 0) return (__errno_set(EINVAL), NULL);

	static unsigned int counter = 0;
	unsigned int attempts = 0;

	while (attempts++ < TMP_MAX) {
		unsigned int seed = (unsigned int)time(NULL)
		                  ^ (unsigned int)getpid()
		                  ^ __jacl_rand_seed
		                  ^ __sync_fetch_and_add(&counter, 1);

		for (int i = 0; i < 6; i++) {
			seed = seed * 1103515245u + 12345u;

			int r = (seed >> 16) % 62;

			if (r < 10) suffix[i] = '0' + r;
			else if (r < 36) suffix[i] = 'A' + (r - 10);
			else suffix[i] = 'a' + (r - 36);
		}

		if (mkdir(template, 0700) == 0) return template;

		if (errno != __errno_val(EEXIST)) return NULL;
	}

	return (__errno_set(EEXIST), NULL);
}

#endif /* JACL_HAS_POSIX */

/* ============================================================= */
/* Pseudo-Terminal (XSI Extension)                               */
/* ============================================================= */
static inline int posix_openpt(int flags) {
	return open("/dev/ptmx", flags);
}

static inline int grantpt(int fd) {
	/* Linux kernel handles permissions automatically; no action needed */
	(void)fd;
	return 0;
}

static inline int unlockpt(int fd) {
	int unlock = 0;
	/* 0 = unlock (make slave available), 1 = lock */
	return ioctl(fd, TIOCSPTLCK, &unlock);
}

static inline int ptsname_r(int fd, char *buf, size_t buflen) {
	int index;

	if (ioctl(fd, TIOCGPTN, &index) < 0) return errno ? errno : EINVAL;
	if ((size_t)snprintf(buf, buflen, "/dev/pts/%d", index) >= buflen) return ERANGE;

	return 0;
}

static inline char *ptsname(int fd) {
	static char buf[64];

	return ptsname_r(fd, buf, sizeof(buf)) == 0 ? buf : NULL;
}

/* ============================================================= */
/* 48-bit Linear Congruential Generator (XSI)                    */
/* ============================================================= */
static inline uint64_t __jacl_rand48_step(unsigned short *xi, unsigned short *lc, unsigned short add) {
	uint64_t x = (uint64_t)xi[0] | ((uint64_t)xi[1] << 16) | ((uint64_t)xi[2] << 32);
	uint64_t m = (uint64_t)lc[0] | ((uint64_t)lc[1] << 16) | ((uint64_t)lc[2] << 32);

	x = (x * m + add) & ((1ULL << 48) - 1);
	xi[0] = x & 0xFFFF; xi[1] = (x >> 16) & 0xFFFF; xi[2] = (x >> 32) & 0xFFFF;

	return x;
}

static inline double erand48(unsigned short xsubi[3]) { return (double)__jacl_rand48_step(xsubi, __jacl_rand48_mult, __jacl_rand48_add) / (1ULL << 48); }
static inline double drand48(void) { return erand48(__jacl_rand48_seed); }
static inline long nrand48(unsigned short xsubi[3]) { return (long)(__jacl_rand48_step(xsubi, __jacl_rand48_mult, __jacl_rand48_add) >> 17); }
static inline long lrand48(void) { return nrand48(__jacl_rand48_seed); }
static inline long jrand48(unsigned short xsubi[3]) { return (long)((int32_t)(__jacl_rand48_step(xsubi, __jacl_rand48_mult, __jacl_rand48_add) >> 16)); }
static inline long mrand48(void) { return jrand48(__jacl_rand48_seed); }
static inline void srand48(long seedval) {
	__jacl_rand48_seed[0] = 0x330E;
	__jacl_rand48_seed[1] = (unsigned short)seedval;
	__jacl_rand48_seed[2] = (unsigned short)(seedval >> 16);
	__jacl_rand48_mult[0] = 0xE66D;
	__jacl_rand48_mult[1] = 0xDEEC;
	__jacl_rand48_mult[2] = 0x0005;
	__jacl_rand48_add = 0x000B;
}
static inline unsigned short *seed48(unsigned short seed16v[3]) {
	__jacl_rand48_oldseed[0] = __jacl_rand48_seed[0];
	__jacl_rand48_oldseed[1] = __jacl_rand48_seed[1];
	__jacl_rand48_oldseed[2] = __jacl_rand48_seed[2];
	__jacl_rand48_seed[0] = seed16v[0];
	__jacl_rand48_seed[1] = seed16v[1];
	__jacl_rand48_seed[2] = seed16v[2];

	return __jacl_rand48_oldseed;
}
static inline void lcong48(unsigned short param[7]) {
	__jacl_rand48_seed[0] = param[0];
	__jacl_rand48_seed[1] = param[1];
	__jacl_rand48_seed[2] = param[2];
	__jacl_rand48_mult[0] = param[3];
	__jacl_rand48_mult[1] = param[4];
	__jacl_rand48_mult[2] = param[5];
	__jacl_rand48_add = param[6];
}

/* ============================================================= */
/* BSD Additive Feedback Generator (XSI)                         */
/* ============================================================= */
static inline long random(void) {
	*__jacl_random_fptr += *__jacl_random_rptr;
	uint_least32_t val = *__jacl_random_fptr;

	if (++__jacl_random_fptr >= &__jacl_random_state[31]) __jacl_random_fptr = __jacl_random_state;
	if (++__jacl_random_rptr >= &__jacl_random_state[31]) __jacl_random_rptr = __jacl_random_state;

	return (long)(val >> 1);
}
static inline void srandom(unsigned seed) {
	__jacl_random_state[0] = seed;

	for (int i = 1; i < 31; i++) __jacl_random_state[i] = (16807ULL * __jacl_random_state[i - 1]) % 2147483647;

	__jacl_random_fptr = &__jacl_random_state[3];
	__jacl_random_rptr = &__jacl_random_state[0];

	for (int i = 0; i < 10 * 31; i++) random();
}

static inline char *initstate(unsigned seed, char *state, size_t size) {
	if (!state || size < 31 * sizeof(uint_least32_t)) return (__errno_set(EINVAL), NULL);

	uint_least32_t *new_state = (uint_least32_t *)state;
	uint_least32_t *old_state = __jacl_random_state;

	// Initialize new state array
	new_state[0] = seed;

	for (int i = 1; i < 31; i++) new_state[i] = (16807ULL * new_state[i - 1]) % 2147483647;

	// Switch to new state
	__jacl_random_state = new_state;
	__jacl_random_fptr = &new_state[3];
	__jacl_random_rptr = &new_state[0];

	// Warm up the generator
	for (int i = 0; i < 10 * 31; i++) random();

	return (char *)old_state;
}
static inline char *setstate(char *state) {
	if (!state) { return (__errno_set(EINVAL), NULL); }

	uint_least32_t *new_state = (uint_least32_t *)state;
	uint_least32_t *old_state = __jacl_random_state;

	// Switch state arrays
	__jacl_random_state = new_state;
	__jacl_random_fptr = &new_state[3];
	__jacl_random_rptr = &new_state[0];

	return (char *)old_state;
}

/* ============================================================= */
/* BASE6 ENCODING PREMITIVES                                     */
/* ============================================================= */
static const char BASE64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static inline long a64l(const char *s) {
	long result = 0, mult = 1;

	for (int i = 0; i < 6 && *s; i++, s++) {
		const char *p = strchr(BASE64, *s);

		if (!p) break;

		result += (p - BASE64) * mult;
		mult *= 64;
	}

	return result;
}
static inline char *l64a(long value) {
	static char buf[7];
	char *p = buf;
	unsigned long v = (unsigned long)value & 0xFFFFFFFFUL;

	for (int i = 0; i < 6 && v; i++, v >>= 6) *p++ = BASE64[v & 0x3F];

	*p = '\0';

	return buf;
}

/* ============================================================= */
/* Remaining CX Extensions (Issue 8)                             */
/* ============================================================= */

static inline int mkostemp(char *template, int flags) {
	if (!template) return (__errno_set(EINVAL), -1);

	size_t len = strlen(template);
	if (len < 6) return (__errno_set(EINVAL), -1);

	char *suffix = template + len - 6;
	if (strcmp(suffix, "XXXXXX") != 0) return (__errno_set(EINVAL), -1);

	static unsigned int counter = 0;
	unsigned int attempts = 0;

	while (attempts++ < TMP_MAX) {
		unsigned int seed = (unsigned int)time(NULL)
		                  ^ (unsigned int)getpid()
		                  ^ __jacl_rand_seed
		                  ^ __sync_fetch_and_add(&counter, 1);

		for (int i = 0; i < 6; i++) {
			seed = seed * 1103515245u + 12345u;
			int r = (seed >> 16) % 62;
			if (r < 10) suffix[i] = '0' + r;
			else if (r < 36) suffix[i] = 'A' + (r - 10);
			else suffix[i] = 'a' + (r - 36);
		}

		int fd = open(template, O_RDWR | O_CREAT | O_EXCL | flags, 0600);
		if (fd >= 0) return fd;
		if (!__errno_chk(EEXIST)) return -1;
	}

	return (__errno_set(EEXIST), -1);
}

static inline char *secure_getenv(const char *name) { return (getuid() != geteuid() || getgid() != getegid()) ? NULL : getenv(name); }

static inline int getsubopt(char **optionp, char *const *tokens, char **valuep) {
	if (!optionp || !*optionp || !tokens || !valuep) return (__errno_set(EINVAL), -1);

	char *start = *optionp;
	char *end = start;

	while (*end && *end != ',') end++;

	*optionp = *end ? end + 1 : end;

	char *eq = start;

	while (eq < end && *eq != '=') eq++;

	char saved = *end;

	*end = '\0';

	for (int i = 0; tokens[i]; i++) {
		size_t tlen = strlen(tokens[i]);

		if (strncmp(start, tokens[i], tlen) == 0) {
			if (start[tlen] == '\0') {
				*valuep = NULL;
				*end = saved;

				return i;
			} else if (start[tlen] == '=') {
				*valuep = &start[tlen + 1];
				*end = saved;

				return i;
			}
		}
	}

	*valuep = start;
	*end = saved;

	return -1;
}

/* ============================================================= */
/* Obsolescent XSI Extensions                                    */
/* ============================================================= */
static inline void setkey(const char *key) {
		/* TODO: Connect to crypto core */
		(void)key;
}

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H */
