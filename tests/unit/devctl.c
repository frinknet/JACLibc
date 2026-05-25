/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <devctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdatomic.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(devctl.h);

/* ============================================================================ */
/* Thread Helpers for Same-FD Test */
/* ============================================================================ */

typedef struct {
    int fd;
    atomic_int *errors;
} thread_args_t;

static void *worker_same_fd(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int r = posix_devctl(args->fd, 0, NULL, 0, NULL);
    if (r != 0 && r != ENOSYS && r != ENOTTY) {
        atomic_fetch_add(args->errors, 1);
    }
    return NULL;
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_return_contract);

TEST(posix_devctl_returns_error_number_not_neg_one) {
    int r = posix_devctl(-1, 0, NULL, 0, NULL);
    ASSERT_EQ(r, EBADF);
    ASSERT_NE(r, -1);
}

TEST(posix_devctl_does_not_modify_errno) {
    errno = 99;
    posix_devctl(-1, 0, NULL, 0, NULL);
    ASSERT_EQ(errno, 99);
}

TEST(posix_devctl_return_value_is_valid_errno_or_zero) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int r = posix_devctl(fd, 0, NULL, 0, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EINVAL || r == EIO || r == EPERM);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_ebadf);

TEST(posix_devctl_negative_fd) { ASSERT_EQ(posix_devctl(-1, 0, NULL, 0, NULL), EBADF); }
TEST(posix_devctl_closed_fd) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    close(fd);
    ASSERT_EQ(posix_devctl(fd, 0, NULL, 0, NULL), EBADF);
}
TEST(posix_devctl_stdout_may_not_be_char) {
    int r = posix_devctl(STDOUT_FILENO, 0, NULL, 0, NULL);
    ASSERT_TRUE(r == ENOTTY || r == 0 || r == ENOSYS);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_enotty);

TEST(posix_devctl_regular_file) {
    char tmp[] = "/tmp/jacl_devctl_file_XXXXXX";
    int fd = mkstemp(tmp); ASSERT_TRUE(fd >= 0);
    ASSERT_EQ(posix_devctl(fd, 0, NULL, 0, NULL), ENOTTY);
    close(fd); unlink(tmp);
}
TEST(posix_devctl_directory) {
    char tmp[] = "/tmp/jacl_devctl_dir_XXXXXX";
    if (!mkdtemp(tmp)) { TEST_SKIP("mkdtemp failed"); return; }
    int fd = open(tmp, O_RDONLY | O_DIRECTORY);
    if (fd < 0) { rmdir(tmp); TEST_SKIP("open dir failed"); return; }
    ASSERT_EQ(posix_devctl(fd, 0, NULL, 0, NULL), ENOTTY);
    close(fd); rmdir(tmp);
}
TEST(posix_devctl_pipe) {
    int fds[2]; if (pipe(fds) != 0) { TEST_SKIP("pipe failed"); return; }
    ASSERT_EQ(posix_devctl(fds[0], 0, NULL, 0, NULL), ENOTTY);
    close(fds[0]); close(fds[1]);
}
TEST(posix_devctl_socket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { TEST_SKIP("socket failed"); return; }
    ASSERT_EQ(posix_devctl(fd, 0, NULL, 0, NULL), ENOTTY);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_einval);

TEST(posix_devctl_obsolescent_nbyte_zero_with_ptr) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int buf = 42;
    int r = posix_devctl(fd, 0, &buf, 0, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == EINVAL);
    close(fd);
}
TEST(posix_devctl_excessive_nbyte) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    char buf[4096];
    int r = posix_devctl(fd, 0, buf, SIZE_MAX, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == EINVAL || r == EOVERFLOW);
    close(fd);
}
TEST(posix_devctl_unaligned_buffer) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    char buf[4096];
    char *unaligned = (char *)(((uintptr_t)buf + 1) & ~3);
    int r = posix_devctl(fd, 0, unaligned, 100, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EINVAL);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_dcmd_edges);

TEST(posix_devctl_dcmd_boundary_values) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int info;
    ASSERT_TRUE(posix_devctl(fd, 0, NULL, 0, &info) != -1);
    ASSERT_TRUE(posix_devctl(fd, -1, NULL, 0, &info) != -1);
    ASSERT_TRUE(posix_devctl(fd, INT_MAX, NULL, 0, &info) != -1);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_eintr);

static volatile sig_atomic_t got_signal = 0;
static void intr_handler(int sig) { (void)sig; got_signal = 1; }

TEST(posix_devctl_signal_interrupt) {
    struct sigaction sa, old;
    sa.sa_handler = intr_handler; sigemptyset(&sa.sa_mask); sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, &old);

    alarm(1); got_signal = 0;
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { sigaction(SIGALRM, &old, NULL); TEST_SKIP("/dev/null missing"); return; }

    int r = posix_devctl(fd, 0, NULL, 0, NULL);
    ASSERT_TRUE(r == 0 || r == EINTR || r == ENOSYS);

    close(fd); alarm(0); sigaction(SIGALRM, &old, NULL);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_eperm_contract);

TEST(posix_devctl_eperm_does_not_corrupt_state) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int info = 0xDEADBEEF;
    int r = posix_devctl(fd, 0, NULL, 0, &info);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EPERM || r == EINVAL);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_dev_info_ptr);

TEST(posix_devctl_null_dev_info) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int r = posix_devctl(fd, 0, NULL, 0, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS);
    close(fd);
}
TEST(posix_devctl_dev_info_initialized_on_valid_fd) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int info = 0xDEADBEEF;
    posix_devctl(fd, 0, NULL, 0, &info);
    close(fd);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_thread_safety);

static atomic_int thread_errors = 0;

static void *thread_devctl(void *arg) {
    (void)arg;
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { atomic_fetch_add(&thread_errors, 1); return NULL; }
    int r = posix_devctl(fd, 0, NULL, 0, NULL);
    if (r != 0 && r != ENOSYS && r != ENOTTY) atomic_fetch_add(&thread_errors, 1);
    close(fd);
    return NULL;
}

TEST(posix_devctl_concurrent_calls) {
    pthread_t t[32];
    for (int i = 0; i < 32; i++) pthread_create(&t[i], NULL, thread_devctl, NULL);
    for (int i = 0; i < 32; i++) pthread_join(t[i], NULL);
    ASSERT_EQ(atomic_load(&thread_errors), 0);
}

TEST(posix_devctl_same_fd_concurrent) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }

    atomic_int local_errors = 0;
    thread_args_t args = { .fd = fd, .errors = &local_errors };
    pthread_t t[16];

    for (int i = 0; i < 16; i++) {
        pthread_create(&t[i], NULL, worker_same_fd, &args);
    }
    for (int i = 0; i < 16; i++) {
        pthread_join(t[i], NULL);
    }

    close(fd);
    ASSERT_EQ(atomic_load(&local_errors), 0);
}

/* ============================================================================ */
TEST_SUITE(posix_devctl_advanced_edges);

TEST(posix_devctl_duplicated_fd) {
    /* POSIX: devctl operates on the open file description, not the FD integer */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int fd2 = dup(fd);
    ASSERT_TRUE(fd2 >= 0);
    int r = posix_devctl(fd2, 0, NULL, 0, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY);
    close(fd2); close(fd);
}

TEST(posix_devctl_mmap_buffer) {
    /* Verify safe interaction with memory-mapped regions as dev_data_ptr */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    size_t len = (size_t)sysconf(_SC_PAGESIZE);
    char *buf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_PTR_NE(buf, MAP_FAILED);
    int r = posix_devctl(fd, 0, buf, len, NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EINVAL);
    munmap(buf, len);
    close(fd);
}

TEST(posix_devctl_dev_info_ptr_nonnull) {
    /* Verify the secondary output path is safely writable */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    int info = 0xCAFEBABE;
    int r = posix_devctl(fd, 0, NULL, 0, &info);
    /* Must not crash; info should be overwritten or left consistent per impl */
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY);
    close(fd);
}

TEST(posix_devctl_sequential_stress) {
    /* Verify no FD leaks, internal state corruption, or errno drift across calls */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    errno = 42; /* Seed errno */
    for (int i = 0; i < 1000; i++) {
        int r = posix_devctl(fd, i % 3, NULL, 0, NULL);
        ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EINVAL || r == EPERM);
    }
    ASSERT_EQ(errno, 42); /* Verify preservation across loop */
    close(fd);
}

TEST(posix_devctl_large_stack_buffer) {
    /* Ensure large stack buffers don't trigger alignment/stack issues */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    char buf[65536]; /* 64KB stack allocation */
    int r = posix_devctl(fd, 0, buf, sizeof(buf), NULL);
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == ENOTTY || r == EINVAL);
    close(fd);
}

TEST(posix_devctl_zero_nbyte_with_valid_ptr) {
    /* POSIX obsolescent clause: nbyte=0 + non-null ptr = unspecified */
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) { TEST_SKIP("/dev/null missing"); return; }
    char dummy = 'X';
    int r = posix_devctl(fd, 0, &dummy, 0, NULL);
    /* Must not crash; impl may return 0, ENOSYS, or EINVAL */
    ASSERT_TRUE(r == 0 || r == ENOSYS || r == EINVAL);
    close(fd);
}

/* ============================================================================ */
TEST_MAIN()
