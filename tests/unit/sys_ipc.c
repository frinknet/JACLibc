/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stddef.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys/ipc.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_ipc_flags) {
    ASSERT_EQ(IPC_CREAT, 01000);
    ASSERT_EQ(IPC_EXCL, 02000);
    ASSERT_EQ(IPC_NOWAIT, 04000);
    
    ASSERT_NE(IPC_CREAT, IPC_EXCL);
    ASSERT_NE(IPC_NOWAIT, IPC_CREAT);
}

TEST(constants_ipc_keys) {
    ASSERT_EQ(IPC_PRIVATE, (key_t)0);
    
    key_t k = IPC_PRIVATE;
    ASSERT_EQ(k, 0);
}

TEST(constants_ipc_commands) {
    ASSERT_EQ(IPC_RMID, 0);
    ASSERT_EQ(IPC_SET, 1);
    ASSERT_EQ(IPC_STAT, 2);
    
    ASSERT_NE(IPC_RMID, IPC_SET);
    ASSERT_NE(IPC_SET, IPC_STAT);
}

/* ============================================================================ */
TEST_SUITE(ipc_perm);

TEST(ipc_perm_members) {
    struct ipc_perm perm;
    
    /* Verify all required members exist */
    ASSERT_OFFSET(struct ipc_perm, uid, offsetof(struct ipc_perm, uid));
    ASSERT_OFFSET(struct ipc_perm, gid, offsetof(struct ipc_perm, gid));
    ASSERT_OFFSET(struct ipc_perm, cuid, offsetof(struct ipc_perm, cuid));
    ASSERT_OFFSET(struct ipc_perm, cgid, offsetof(struct ipc_perm, cgid));
    ASSERT_OFFSET(struct ipc_perm, mode, offsetof(struct ipc_perm, mode));
    
    /* Verify types */
    ASSERT_SIZE(perm.uid, sizeof(uid_t));
    ASSERT_SIZE(perm.gid, sizeof(gid_t));
    ASSERT_SIZE(perm.cuid, sizeof(uid_t));
    ASSERT_SIZE(perm.cgid, sizeof(gid_t));
    ASSERT_SIZE(perm.mode, sizeof(mode_t));
}

TEST(ipc_perm_initialization) {
    struct ipc_perm perm = {0};
    
    ASSERT_EQ(perm.uid, 0);
    ASSERT_EQ(perm.gid, 0);
    ASSERT_EQ(perm.cuid, 0);
    ASSERT_EQ(perm.cgid, 0);
    ASSERT_EQ(perm.mode, 0);
}

/* ============================================================================ */
TEST_SUITE(ftok);

TEST(ftok_signature) {
    /* Verify ftok is declared */
    key_t (*fp)(const char *, int) = ftok;
    ASSERT_NOT_NULL(fp);
}

TEST(ftok_null_path) {
    errno = 0;
    key_t k = ftok(NULL, 1);
    ASSERT_EQ(k, (key_t)-1);
    /* stat usually sets EFAULT for NULL, but ENOENT is also possible depending on impl */
    ASSERT_TRUE(errno == EFAULT || errno == EINVAL || errno == ENOENT);
}

TEST(ftok_nonexistent_path) {
    errno = 0;
    key_t k = ftok("/this/path/does/not/exist/ever", 1);
    ASSERT_EQ(k, (key_t)-1);
    ASSERT_ERRNO(ENOENT);
}

TEST(ftok_valid_path) {
    /* Use /tmp which should exist */
    key_t k = ftok("/tmp", 1);
    ASSERT_NE(k, (key_t)-1);
    ASSERT_NE(k, 0); /* Should be a valid key */
}

TEST(ftok_different_ids) {
    key_t k1 = ftok("/tmp", 1);
    key_t k2 = ftok("/tmp", 2);
    
    ASSERT_NE(k1, (key_t)-1);
    ASSERT_NE(k2, (key_t)-1);
    ASSERT_NE(k1, k2); /* Different IDs should yield different keys */
}

TEST(ftok_zero_id) {
    key_t k = ftok("/tmp", 0);
    ASSERT_NE(k, (key_t)-1);
    /* Key can be valid even with ID 0, as long as path is valid */
}

/* ============================================================================ */
TEST_MAIN()
