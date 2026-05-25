/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

TEST_TYPE(unit);
TEST_UNIT(syslog.h);

/* ============================================================================ */
TEST_SUITE(syslog_constants);

TEST(log_priorities) {
    ASSERT_INT_EQ(LOG_EMERG, 0);
    ASSERT_INT_EQ(LOG_ALERT, 1);
    ASSERT_INT_EQ(LOG_CRIT, 2);
    ASSERT_INT_EQ(LOG_ERR, 3);
    ASSERT_INT_EQ(LOG_WARNING, 4);
    ASSERT_INT_EQ(LOG_NOTICE, 5);
    ASSERT_INT_EQ(LOG_INFO, 6);
    ASSERT_INT_EQ(LOG_DEBUG, 7);
}

TEST(log_facilities) {
    ASSERT_INT_EQ(LOG_KERN, 0);
    ASSERT_INT_EQ(LOG_USER, 8);
    ASSERT_INT_EQ(LOG_MAIL, 16);
    ASSERT_INT_EQ(LOG_DAEMON, 24);
    ASSERT_INT_EQ(LOG_AUTH, 32);
    ASSERT_INT_EQ(LOG_SYSLOG, 40);
    ASSERT_INT_EQ(LOG_LOCAL0, 128);
}

TEST(log_macros) {
    ASSERT_INT_EQ(LOG_PRI(LOG_ERR | LOG_USER), LOG_ERR);
    ASSERT_INT_EQ(LOG_FAC(LOG_ERR | LOG_USER), LOG_USER);
    ASSERT_INT_EQ(LOG_MAKEPRI(LOG_LOCAL0, LOG_DEBUG), 128 | 7);
}

TEST(log_mask_macros) {
    ASSERT_INT_EQ(LOG_MASK(LOG_ERR), (1 << 3));
    ASSERT_INT_EQ(LOG_UPTO(LOG_ERR), (1 << 4) - 1); // 0-3 set
    ASSERT_INT_EQ(LOG_UPTO(LOG_DEBUG), 0xFF); // 0-7 set
}

/* ============================================================================ */
TEST_SUITE(syslog_lifecycle);

TEST(openlog_null_ident) {
    openlog(NULL, LOG_PID, LOG_USER);
    ASSERT_TRUE(1); // Should not crash
    closelog();
}

TEST(openlog_ndelay) {
    closelog();
    openlog("ndelay_test", LOG_NDELAY, LOG_USER);
    ASSERT_TRUE(1);
    closelog();
}

TEST(closelog_multiple) {
    closelog();
    closelog();
    closelog();
    ASSERT_TRUE(1);
}

TEST(setlogmask_restore) {
    int old = setlogmask(0);
    int new = setlogmask(old);
    ASSERT_INT_EQ(new, 0);
}

/* ============================================================================ */
TEST_SUITE(syslog_sad_paths);

TEST(syslog_null_format) {
    setlogmask(0xFF);
    syslog(LOG_INFO, NULL);
    ASSERT_TRUE(1);
}

TEST(syslog_mask_emerg_only) {
    setlogmask(LOG_MASK(LOG_EMERG));
    syslog(LOG_DEBUG, "Masked");
    syslog(LOG_INFO, "Masked");
    setlogmask(0xFF);
    ASSERT_TRUE(1);
}

TEST(syslog_special_chars) {
    syslog(LOG_INFO, "Percent %% Newline \n Tab \t End");
    ASSERT_TRUE(1);
}

TEST(syslog_empty_message) {
    syslog(LOG_INFO, "");
    ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_MAIN()
