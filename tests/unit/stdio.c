/* (c) 2025 FRINKnet & Friends – MIT licence */
#include "../runner.h"
#include <stdio.h>

TEST_SUITE(stdio);

TEST(sprinf_basic_output) {
		char buf[64];
		int ret = sprintf(buf, "hello");
		
		ASSERT_STR_EQ("hello", buf);
		ASSERT_EQ(5, ret);
}

TEST(sprintf_percent_percent) {
		char buf[64];
		int ret = sprintf(buf, "%%");
		
		ASSERT_STR_EQ("%", buf);
		ASSERT_EQ(1, ret);
}

TEST_MAIN()

