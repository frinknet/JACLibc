/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <sys/shm.h>
#include <sys/ipc.h>
#include <testing.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(sys_shm.h);

static void __test_shm_cleanup(key_t key) {
    int id = shmget(key, 0, 0);
    if (id >= 0) shmctl(id, IPC_RMID, NULL);
    char path[256];
    snprintf(path, sizeof(path), "/tmp/jacl_sysv_shm_%d", (int)key);
    unlink(path);
}

/* ============================================================================ */

TEST_SUITE(shmatt_t);
TEST(shmatt_t_size) {
    shmatt_t x = 0;
    ASSERT_INT_EQ(sizeof(x), sizeof(unsigned long));
}

/* ============================================================================ */

TEST_SUITE(shmid_ds);
TEST(shmid_ds_init) {
    struct shmid_ds ds = {0};
    ds.shm_perm.uid = 1000;
    ds.shm_perm.gid = 1000;
    ds.shm_perm.mode = 0644;
    ds.shm_segsz = 8192;
    ds.shm_nattch = 5;
    ds.shm_cpid = getpid();

    ASSERT_INT_EQ(ds.shm_perm.uid, 1000);
    ASSERT_INT_EQ(ds.shm_perm.mode, 0644);
    ASSERT_INT_EQ(ds.shm_segsz, 8192);
    ASSERT_INT_EQ(ds.shm_nattch, 5);
    ASSERT_INT_EQ(ds.shm_cpid, getpid());
}

/* ============================================================================ */

TEST_SUITE(shmget);
TEST(shmget_create_private) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), 0);
}
TEST(shmget_create_named) {
    __test_shm_cleanup(100001);
    int id = shmget(100001, 4096, IPC_CREAT | IPC_EXCL | 0600);
    ASSERT_TRUE(id >= 0);
    ASSERT_INT_EQ(shmget(100001, 0, 0), id);
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), 0);
}
TEST(shmget_excl_fails_if_exists) {
    __test_shm_cleanup(100002);
    int id = shmget(100002, 4096, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    errno = 0;
    int id2 = shmget(100002, 4096, IPC_CREAT | IPC_EXCL | 0600);
    ASSERT_INT_EQ(id2, -1);
    ASSERT_INT_EQ(errno, EEXIST);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmget_zero_size_create_fails) {
    errno = 0;
    int id = shmget(IPC_PRIVATE, 0, IPC_CREAT | 0600);
    ASSERT_INT_EQ(id, -1);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmget_no_create_fails_if_missing) {
    __test_shm_cleanup(100003);
    errno = 0;
    int id = shmget(100003, 4096, 0600);
    ASSERT_INT_EQ(id, -1);
    ASSERT_INT_EQ(errno, ENOENT);
}
TEST(shmget_zero_size_query_succeeds) {
    __test_shm_cleanup(100004);
    int id = shmget(100004, 4096, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    int id2 = shmget(100004, 0, 0);
    ASSERT_INT_EQ(id, id2);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmget_private_excl_always_creates) {
    int id1 = shmget(IPC_PRIVATE, 4096, IPC_CREAT | IPC_EXCL | 0600);
    int id2 = shmget(IPC_PRIVATE, 4096, IPC_CREAT | IPC_EXCL | 0600);
    ASSERT_TRUE(id1 >= 0);
    ASSERT_TRUE(id2 >= 0);
    ASSERT_TRUE(id1 != id2);
    shmctl(id1, IPC_RMID, NULL);
    shmctl(id2, IPC_RMID, NULL);
}
TEST(shmget_smaller_size_query_succeeds) {
    __test_shm_cleanup(100010);
    int id = shmget(100010, 8192, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    int id2 = shmget(100010, 4096, 0);
    ASSERT_INT_EQ(id, id2);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmget_larger_size_query_fails) {
    __test_shm_cleanup(100011);
    int id = shmget(100011, 4096, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    errno = 0;
    int id2 = shmget(100011, 8192, 0);
    ASSERT_INT_EQ(id2, -1);
    ASSERT_INT_EQ(errno, EINVAL);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmget_registry_exhaustion) {
    int ids[JACL_SHM_MAX_SEGMENTS];
    int count = 0;
    for (int i = 0; i < JACL_SHM_MAX_SEGMENTS; i++) {
        ids[i] = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
        if (ids[i] < 0) break;
        count++;
    }
    ASSERT_INT_EQ(count, JACL_SHM_MAX_SEGMENTS);
    errno = 0;
    int extra = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    ASSERT_INT_EQ(extra, -1);
    ASSERT_INT_EQ(errno, ENOSPC);
    for (int i = 0; i < count; i++) {
        shmctl(ids[i], IPC_RMID, NULL);
    }
}

/* ============================================================================ */

TEST_SUITE(shmat);
TEST(shmat_basic) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    ASSERT_TRUE(id >= 0);
    void *addr = shmat(id, NULL, 0);
    ASSERT_TRUE(addr != SHM_FAILED);
    strcpy((char *)addr, "hello shm");
    shmdt(addr);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmat_rdonly) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *addr = shmat(id, NULL, SHM_RDONLY);
    ASSERT_TRUE(addr != SHM_FAILED);
    shmdt(addr);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmat_multiple_attachments) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *a1 = shmat(id, NULL, 0);
    void *a2 = shmat(id, NULL, 0);
    ASSERT_PTR_EQ(a1, a2);
    shmdt(a1);
    shmdt(a2);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmat_explicit_addr_fails_without_remap) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *dummy = (void *)0x10000000;
    errno = 0;
    void *addr = shmat(id, dummy, 0);
    ASSERT_PTR_EQ(addr, SHM_FAILED);
    ASSERT_INT_EQ(errno, EINVAL);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmat_invalid_id) {
    errno = 0;
    void *addr = shmat(999999, NULL, 0);
    ASSERT_PTR_EQ(addr, SHM_FAILED);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmat_data_persistence) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *a1 = shmat(id, NULL, 0);
    strcpy((char*)a1, "persistence_test");
    shmdt(a1);
    void *a2 = shmat(id, NULL, 0);
    ASSERT_STR_EQ((char*)a2, "persistence_test");
    shmdt(a2);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmat_named_cross_attach) {
    __test_shm_cleanup(100005);
    int id1 = shmget(100005, 4096, IPC_CREAT | 0600);
    void *a1 = shmat(id1, NULL, 0);
    strcpy((char*)a1, "named_data");
    int id2 = shmget(100005, 0, 0);
    ASSERT_INT_EQ(id1, id2);
    void *a2 = shmat(id2, NULL, 0);
    ASSERT_STR_EQ((char*)a2, "named_data");
    shmdt(a1);
    shmdt(a2);
    shmctl(id1, IPC_RMID, NULL);
}
TEST(shmat_shm_failed) {
    errno = 0;
    void *addr = shmat(999999, NULL, 0);
    ASSERT_PTR_EQ(addr, SHM_FAILED);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmat_shm_rnd) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *addr = shmat(id, NULL, 0);
    ASSERT_TRUE(addr != SHM_FAILED);

    uintptr_t base = (uintptr_t)addr;
    uintptr_t offset_addr = base + (SHMLBA / 2);

    void *addr2 = shmat(id, (void *)offset_addr, SHM_RND);
    ASSERT_PTR_EQ(addr2, addr);

    shmdt(addr);
    shmdt(addr2);
    shmctl(id, IPC_RMID, NULL);
}

/* ============================================================================ */

TEST_SUITE(shmdt);
TEST(shmdt_basic) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *addr = shmat(id, NULL, 0);
    ASSERT_INT_EQ(shmdt(addr), 0);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmdt_twice_fails) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *addr = shmat(id, NULL, 0);
    ASSERT_INT_EQ(shmdt(addr), 0);
    errno = 0;
    ASSERT_INT_EQ(shmdt(addr), -1);
    ASSERT_INT_EQ(errno, EINVAL);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmdt_invalid_addr) {
    errno = 0;
    int ret = shmdt((void *)0x12345678);
    ASSERT_INT_EQ(ret, -1);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmdt_nattch_tracking) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *a1 = shmat(id, NULL, 0);
    void *a2 = shmat(id, NULL, 0);
    struct shmid_ds ds;
    shmctl(id, IPC_STAT, &ds);
    ASSERT_INT_EQ(ds.shm_nattch, 2);
    shmdt(a1);
    shmctl(id, IPC_STAT, &ds);
    ASSERT_INT_EQ(ds.shm_nattch, 1);
    shmdt(a2);
    shmctl(id, IPC_STAT, &ds);
    ASSERT_INT_EQ(ds.shm_nattch, 0);
    shmctl(id, IPC_RMID, NULL);
}

/* ============================================================================ */

TEST_SUITE(shmctl);
TEST(shmctl_info) {
    struct shmid_ds ds;
    ASSERT_INT_EQ(shmctl(0, IPC_INFO, &ds), 0);
    ASSERT_INT_EQ(ds.shm_segsz, JACL_SHM_MAX_SEGMENTS);
}
TEST(shmctl_stat) {
    int id = shmget(IPC_PRIVATE, 8192, IPC_CREAT | 0644);
    void *addr = shmat(id, NULL, 0);
    struct shmid_ds ds;
    ASSERT_INT_EQ(shmctl(id, IPC_STAT, &ds), 0);
    ASSERT_INT_EQ(ds.shm_segsz, 8192);
    ASSERT_INT_EQ(ds.shm_nattch, 1);
    ASSERT_INT_EQ(ds.shm_cpid, getpid());
    shmdt(addr);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmctl_stat_returns_id) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    struct shmid_ds ds;
    int ret = shmctl(id, SHM_STAT, &ds);
    ASSERT_INT_EQ(ret, id);
    ASSERT_INT_EQ(ds.shm_segsz, 4096);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmctl_set) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    struct shmid_ds ds;
    shmctl(id, IPC_STAT, &ds);
    ds.shm_perm.mode = 0666;
    ASSERT_INT_EQ(shmctl(id, IPC_SET, &ds), 0);
    struct shmid_ds ds2;
    shmctl(id, IPC_STAT, &ds2);
    ASSERT_INT_EQ(ds2.shm_perm.mode & 0777, 0666);
    shmctl(id, IPC_RMID, NULL);
}
TEST(shmctl_rmid_deferred) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    void *addr = shmat(id, NULL, 0);
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), 0);
    errno = 0;
    void *addr2 = shmat(id, NULL, 0);
    ASSERT_PTR_EQ(addr2, SHM_FAILED);
    ASSERT_INT_EQ(errno, EIDRM);
    ASSERT_INT_EQ(shmdt(addr), 0);
    errno = 0;
    ASSERT_INT_EQ(shmctl(id, IPC_STAT, NULL), -1);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmctl_rmid_immediate) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), 0);
    errno = 0;
    ASSERT_INT_EQ(shmctl(id, IPC_STAT, NULL), -1);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmctl_rmid_twice_fails) {
    int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), 0);
    errno = 0;
    ASSERT_INT_EQ(shmctl(id, IPC_RMID, NULL), -1);
    ASSERT_INT_EQ(errno, EINVAL);
}
TEST(shmctl_timestamps) {
	int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
	struct shmid_ds ds1;
	shmctl(id, IPC_STAT, &ds1);

	sleep(1);
	void *addr = shmat(id, NULL, 0);

	struct shmid_ds ds2;
	shmctl(id, IPC_STAT, &ds2);
	ASSERT_TRUE(ds2.shm_atime > ds1.shm_ctime);

	sleep(1);
	shmdt(addr);

	struct shmid_ds ds3;
	shmctl(id, IPC_STAT, &ds3);
	ASSERT_TRUE(ds3.shm_dtime > ds2.shm_atime);

	sleep(1);
	ds3.shm_perm.mode = 0666;
	shmctl(id, IPC_SET, &ds3);

	struct shmid_ds ds4;
	shmctl(id, IPC_STAT, &ds4);
	ASSERT_TRUE(ds4.shm_ctime > ds3.shm_dtime);

	shmctl(id, IPC_RMID, NULL);
}
TEST(shmctl_shm_info) {
	struct shmid_ds ds;
	ASSERT_INT_EQ(shmctl(0, SHM_INFO, &ds), 0);
	ASSERT_INT_EQ(ds.shm_segsz, JACL_SHM_MAX_SEGMENTS);
}

TEST_MAIN()
