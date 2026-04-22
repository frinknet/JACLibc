/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <net/telnet.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/telnet.h);

/* ============================================================================ */
TEST_SUITE(command);

TEST(command_iac) { ASSERT_EQ(255, IAC); }

TEST(command_dont) { ASSERT_EQ(254, DONT); }

TEST(command_do) { ASSERT_EQ(253, DO); }

TEST(command_wont) { ASSERT_EQ(252, WONT); }

TEST(command_will) { ASSERT_EQ(251, WILL); }

TEST(command_sb) { ASSERT_EQ(250, SB); }

TEST(command_se) { ASSERT_EQ(240, SE); }

TEST(command_ga) { ASSERT_EQ(249, GA); }

TEST(command_ec) { ASSERT_EQ(247, EC); }

TEST(command_ao) { ASSERT_EQ(245, AO); }

TEST(command_ip) { ASSERT_EQ(244, IP); }

TEST(command_dm) { ASSERT_EQ(242, DM); }

TEST(command_nop) { ASSERT_EQ(241, NOP); }

/* ============================================================================ */

TEST_SUITE(telqual);

TEST(telqual_is) { ASSERT_EQ(0, TELQUAL_IS); }

TEST(telqual_send) { ASSERT_EQ(1, TELQUAL_SEND); }

TEST(telqual_info) { ASSERT_EQ(2, TELQUAL_INFO); }

TEST(telqual_name) { ASSERT_EQ(3, TELQUAL_NAME); }

/* ============================================================================ */

TEST_SUITE(telopt);

TEST(telopt_binary) { ASSERT_EQ(0, TELOPT_BINARY); }

TEST(telopt_echo) { ASSERT_EQ(1, TELOPT_ECHO); }

TEST(telopt_sga) { ASSERT_EQ(3, TELOPT_SGA); }

TEST(telopt_ttype) { ASSERT_EQ(23, TELOPT_TTYPE); }

TEST(telopt_naws) { ASSERT_EQ(31, TELOPT_NAWS); }

TEST(telopt_linemode) { ASSERT_EQ(34, TELOPT_LINEMODE); }

TEST(telopt_starttls) { ASSERT_EQ(40, TELOPT_STARTTLS); }

/* ============================================================================ */

TEST_SUITE(telnet_is_cmd);

TEST(telnet_is_cmd_lower_bound) { ASSERT_TRUE(telnet_is_cmd(SE)); }

TEST(telnet_is_cmd_upper_bound) { ASSERT_TRUE(telnet_is_cmd(IAC)); }

TEST(telnet_is_cmd_mid_range) { ASSERT_TRUE(telnet_is_cmd(GA)); }

TEST(telnet_is_cmd_below_range) { ASSERT_FALSE(!!telnet_is_cmd(SE - 1)); }

/* ============================================================================ */

TEST_SUITE(telnet_is_opt);

TEST(telnet_is_opt_lower_bound) { ASSERT_TRUE(telnet_is_opt(0)); }

TEST(telnet_is_opt_assigned_max) { ASSERT_TRUE(telnet_is_opt(TELOPT_FORWARD_X)); }

TEST(telnet_is_opt_unassigned) { ASSERT_FALSE(telnet_is_opt(44)); }

TEST(telnet_is_opt_iac_boundary) { ASSERT_TRUE(telnet_is_opt(IAC)); }

TEST_MAIN()

#else

int main(void) { printf("net/telnet.h requires POSIX\n"); return 0; }

#endif
