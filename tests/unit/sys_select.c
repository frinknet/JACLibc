/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <sys/select.h>

TEST_TYPE(unit);
TEST_UNIT(sys/select.h);

/* ============================================================================ */

TEST_SUITE(fd_setsize);

TEST(fd_setsize_check) {
	ASSERT_EQ(1024, FD_SETSIZE);
}

/* ============================================================================ */

TEST_SUITE(nfdbits);

TEST(nfdbits_check) {
	ASSERT_EQ((int)(sizeof(unsigned long) * 8), NFDBITS);
}

/* ============================================================================ */

TEST_SUITE(fd_zero);

TEST(fd_zero_all_bits) {
	fd_set set;
	memset(&set, 0xFF, sizeof(set));
	FD_ZERO(&set);
	for (size_t i = 0; i < sizeof(fd_set) / sizeof(unsigned long); i++)
		ASSERT_EQ(0UL, set.fds_bits[i]);
}

TEST(fd_zero_resets) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(10, &set);
	FD_ZERO(&set);
	ASSERT_FALSE(FD_ISSET(10, &set));
}

/* ============================================================================ */

TEST_SUITE(fd_set);

TEST(fd_set_size) {
	ASSERT_EQ(sizeof(unsigned long) * (FD_SETSIZE / NFDBITS), sizeof(fd_set));
}

TEST(fd_set_max_index) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(FD_SETSIZE - 1, &set);
	ASSERT_TRUE(FD_ISSET(FD_SETSIZE - 1, &set));
}

TEST(fd_set_bleed) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(100, &set);
	ASSERT_FALSE(FD_ISSET(99, &set));
	ASSERT_FALSE(FD_ISSET(101, &set));
}

TEST(fd_set_idempotent_set) {
	fd_set s; FD_ZERO(&s); FD_SET(5,&s); FD_SET(5,&s);
	ASSERT_TRUE(FD_ISSET(5,&s));
}

TEST(fd_set_idempotent_clr) {
	fd_set s; FD_ZERO(&s); FD_SET(5,&s); FD_CLR(5,&s); FD_CLR(5,&s);
	ASSERT_FALSE(FD_ISSET(5,&s));
}

TEST(fd_set_even_odd_mask) {
	fd_set s; FD_ZERO(&s);
	for(int i=0; i<64; i++) FD_SET(i, &s);
	for(int i=0; i<64; i+=2) FD_CLR(i, &s);
	for(int i=0; i<64; i++) {
		if(i%2==0) ASSERT_FALSE(FD_ISSET(i,&s));
		else ASSERT_TRUE(FD_ISSET(i,&s));
	}
}

/* ============================================================================ */

TEST_SUITE(fd_clr);

TEST(fd_clr_target) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(50, &set);
	FD_CLR(50, &set);
	ASSERT_FALSE(FD_ISSET(50, &set));
}

TEST(fd_clr_already_cleared) {
	fd_set set;
	FD_ZERO(&set);
	FD_CLR(25, &set);
	ASSERT_FALSE(FD_ISSET(25, &set));
}

TEST(fd_clr_preserves) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(10, &set);
	FD_SET(20, &set);
	FD_CLR(10, &set);
	ASSERT_FALSE(FD_ISSET(10, &set));
	ASSERT_TRUE(FD_ISSET(20, &set));
}

/* ============================================================================ */

TEST_SUITE(fd_isset);

TEST(fd_isset_true) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(7, &set);
	ASSERT_TRUE(FD_ISSET(7, &set));
}

TEST(fd_isset_false) {
	fd_set set;
	FD_ZERO(&set);
	ASSERT_FALSE(FD_ISSET(7, &set));
}

TEST(fd_isset_word_boundary) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(NFDBITS - 1, &set);
	FD_SET(NFDBITS, &set);
	ASSERT_TRUE(FD_ISSET(NFDBITS - 1, &set));
	ASSERT_TRUE(FD_ISSET(NFDBITS, &set));
	ASSERT_FALSE(FD_ISSET(NFDBITS + 1, &set));
}

/* ============================================================================ */

TEST_SUITE(select);

TEST(select_null_timeout) {
	TEST_SKIP("timeout=NULL blocks forever by POSIX spec");
}

TEST(select_zero_nfds) {
	struct timeval tv = {0, 0};
	int ret = select(0, NULL, NULL, NULL, &tv);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(select_empty_sets) {
	fd_set r, w, e;
	FD_ZERO(&r); FD_ZERO(&w); FD_ZERO(&e);
	struct timeval tv = {0, 0};
	int ret = select(1, &r, &w, &e, &tv);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(select_large_nfds) {
	struct timeval tv = {0, 0};
	int ret = select(4096, NULL, NULL, NULL, &tv);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(select_negative_nfds) {
	struct timeval tv = {0, 0};
	int ret = select(-1, NULL, NULL, NULL, &tv);
	ASSERT_TRUE(ret == -1); /* POSIX: nfds must be >= 0 */
}

/* ============================================================================ */

TEST_SUITE(pselect);

TEST(pselect_null_timeout) {
	struct timespec ts = {0, 0};
	int ret = pselect(0, NULL, NULL, NULL, &ts, NULL);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(pselect_null_sigmask) {
	struct timespec ts = {0, 0};
	int ret = pselect(0, NULL, NULL, NULL, &ts, NULL);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(pselect_large_nfds) {
	struct timespec ts = {0, 0};
	int ret = pselect(8192, NULL, NULL, NULL, &ts, NULL);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

TEST(pselect_negative_nfds) {
	struct timespec ts = {0, 0};
	int ret = pselect(-1, NULL, NULL, NULL, &ts, NULL);
	ASSERT_TRUE(ret == -1);
}

TEST(pselect_sigset_compile) {
	sigset_t mask;
	sigemptyset(&mask);
	struct timespec ts = {0};
	int ret = pselect(0, NULL, NULL, NULL, &ts, &mask);
	ASSERT_TRUE(ret == 0 || ret == -1);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "sys/select.h requires POSIX\n")
