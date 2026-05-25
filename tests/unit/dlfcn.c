/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(dlfcn.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_rtld_lazy) {
    ASSERT_INT_NE(RTLD_LAZY, 0);
}

TEST(constants_rtld_now) {
    ASSERT_INT_NE(RTLD_NOW, 0);
}

TEST(constants_rtld_global) {
    ASSERT_INT_NE(RTLD_GLOBAL, 0);
}

TEST(constants_rtld_local) {
    ASSERT_INT_EQ(RTLD_LOCAL, 0);
}

TEST(constants_rtld_default) {
    ASSERT_NULL(RTLD_DEFAULT);
}

TEST(constants_rtld_next) {
    ASSERT_PTR_EQ(RTLD_NEXT, (void *)-1);
}

/* ============================================================================ */
TEST_SUITE(dl_info);

TEST(dl_info_size) {
    ASSERT_SIZE_MIN(Dl_info, sizeof(void *) * 4);
}

TEST(dl_info_offset_dli_fname) {
    ASSERT_OFFSET(Dl_info, dli_fname, 0);
}

TEST(dl_info_offset_dli_fbase) {
    ASSERT_OFFSET_PAST(Dl_info, dli_fbase, sizeof(void *));
}

/* ============================================================================ */
TEST_SUITE(dlopen);

TEST(dlopen_null_returns_default) {
    void *h = dlopen(NULL, RTLD_NOW);
    ASSERT_PTR_EQ(h, RTLD_DEFAULT);
}

TEST(dlopen_clears_error_on_success) {
    dlopen(NULL, RTLD_LAZY);
    char *err = dlerror();
    ASSERT_NULL(err);
}

TEST(dlopen_invalid_path_returns_null) {
    void *h = dlopen("/nonexistent/lib.so", RTLD_NOW);
    ASSERT_NULL(h);
}

TEST(dlopen_invalid_path_sets_error) {
    dlopen("/nonexistent/lib.so", RTLD_NOW);
    char *err = dlerror();
    ASSERT_NOT_NULL(err);
    ASSERT_STR_HAS("open", err);
}

/* ============================================================================ */
TEST_SUITE(dlsym);

TEST(dlsym_null_symbol_returns_null) {
    void *sym = dlsym(RTLD_DEFAULT, NULL);
    ASSERT_NULL(sym);
}

TEST(dlsym_null_symbol_sets_errno) {
    dlsym(RTLD_DEFAULT, NULL);
    ASSERT_ERRNO(EINVAL);
}

TEST(dlsym_rtld_next_unsupported) {
    void *sym = dlsym(RTLD_NEXT, "foo");
    ASSERT_NULL(sym);
}

TEST(dlsym_rtld_next_sets_enosys) {
    dlsym(RTLD_NEXT, "foo");
    ASSERT_ERRNO(ENOSYS);
}

TEST(dlsym_unknown_symbol_returns_null) {
    void *sym = dlsym(RTLD_DEFAULT, "__jacl_nonexistent_xyz");
    ASSERT_NULL(sym);
}

TEST(dlsym_unknown_symbol_sets_fault) {
    dlsym(RTLD_DEFAULT, "__jacl_nonexistent_xyz");
    ASSERT_ERRNO(EFAULT);
}

/* ============================================================================ */
TEST_SUITE(dlerror);

TEST(dlerror_null_after_success) {
    dlopen(NULL, RTLD_LAZY);
    char *err = dlerror();
    ASSERT_NULL(err);
}

TEST(dlerror_returns_string_on_failure) {
    dlsym(RTLD_DEFAULT, "bogus");
    char *err = dlerror();
    ASSERT_NOT_NULL(err);
    ASSERT_STR_HAS("not found", err);
}

/* ============================================================================ */
TEST_SUITE(dlclose);

TEST(dlclose_default_returns_zero) {
    ASSERT_INT_EQ(dlclose(RTLD_DEFAULT), 0);
}

TEST(dlclose_next_returns_zero) {
    ASSERT_INT_EQ(dlclose(RTLD_NEXT), 0);
}

TEST(dlclose_invalid_handle_returns_neg_one) {
    ASSERT_INT_EQ(dlclose((void *)0xBADC0DE), -1);
}

TEST(dlclose_invalid_handle_sets_error) {
    dlclose((void *)0xBADC0DE);
    char *err = dlerror();
    ASSERT_NOT_NULL(err);
    ASSERT_STR_HAS("invalid handle", err);
}

/* ============================================================================ */
TEST_SUITE(dladdr);

TEST(dladdr_returns_zero) {
    Dl_info info;
    int r = dladdr((void *)main, &info);
    ASSERT_INT_EQ(r, 0);
}

TEST(dladdr_does_not_set_errno) {
	int saved = errno;
	errno = 0;
	Dl_info info;
	int r = dladdr((void *)0x12345678, &info);
	ASSERT_INT_EQ(r, 0);
	ASSERT_ERRNO(0); /* POSIX: errno untouched */
	errno = saved;
}

TEST(dladdr_null_info_returns_zero) {
	int r = dladdr((void *)main, NULL);
	ASSERT_INT_EQ(r, 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(dladdr_address_not_in_loaded_object) {
	Dl_info info;
	int r = dladdr((void *)0x12345678, &info);
	ASSERT_INT_EQ(r, 0);
}

TEST(dladdr_output_initialized_on_failure) {
	Dl_info info = { (void *)0xDEADBEEF, (void *)0xDEADBEEF, (void *)0xDEADBEEF, (void *)0xDEADBEEF };
	dladdr((void *)0x12345678, &info);
	ASSERT_NULL(info.dli_fname);
	ASSERT_NULL(info.dli_fbase);
	ASSERT_NULL(info.dli_sname);
	ASSERT_NULL(info.dli_saddr);
}

TEST(dladdr_finds_symbol_in_loaded_handle) {
	char exe_path[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
	if (len <= 0) TEST_SKIP("readlink failed");
	exe_path[len] = '\0';

	char *dir = dirname(exe_path);
	char lib_path[PATH_MAX];
	snprintf(lib_path, sizeof(lib_path), "%s/../lib/libjaclibc.so", dir);

	void *h = dlopen(lib_path, RTLD_NOW);
	if (!h) TEST_SKIP("dlopen failed");

	void *sym = dlsym(h, "aligned_alloc");
	if (!sym) { dlclose(h); TEST_SKIP("symbol not found"); }

	Dl_info info;
	ASSERT_INT_EQ(dladdr(sym, &info), 1);
	ASSERT_TRUE(info.dli_fname != NULL);
	ASSERT_TRUE(info.dli_fbase != NULL);
	dlclose(h);
}

/* ============================================================================ */
TEST_MAIN()
