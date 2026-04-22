/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <net/if.h>

TEST_TYPE(unit);
TEST_UNIT(net/if.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_sizes) {
	ASSERT_EQ(16, IFNAMSIZ);
	ASSERT_EQ(256, IFALIASZ);
}

TEST(constants_flags) {
	ASSERT_EQ(0x1, IFF_UP);
	ASSERT_EQ(0x2, IFF_BROADCAST);
	ASSERT_EQ(0x8, IFF_LOOPBACK);
	ASSERT_EQ(0x40, IFF_RUNNING);
	ASSERT_EQ(0x1000, IFF_MULTICAST);
	ASSERT_EQ(0x40000, IFF_ECHO);
}

TEST(constants_ioctls) {
	ASSERT_EQ(0x8912, SIOCGIFCONF);
	ASSERT_EQ(0x8915, SIOCGIFFLAGS);
	ASSERT_EQ(0x8917, SIOCGIFADDR);
	ASSERT_EQ(0x8921, SIOCGIFMTU);
	ASSERT_EQ(0x8933, SIOCGIFINDEX);
}

/* ============================================================================ */

TEST_SUITE(ifreq);

TEST(ifreq_layout) {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ASSERT_OFFSET(struct ifreq, ifr_name, 0);
	ASSERT_SIZE(ifr.ifr_name, IFNAMSIZ);
	ASSERT_OFFSET(struct ifreq, ifr_ifru.ifru_addr, IFNAMSIZ);
	ASSERT_OFFSET(struct ifreq, ifr_ifru.ifru_flags, IFNAMSIZ);
}

TEST(ifreq_union_overlap) {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = 0x1234;
	ASSERT_EQ(0x1234, ifr.ifr_ifru.ifru_ifindex); /* Short shares union space */
}

TEST(ifreq_name_truncation) {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, "this_is_way_too_long_for_ifnamsiz", IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ASSERT_EQ('\0', ifr.ifr_name[IFNAMSIZ - 1]);
}

/* ============================================================================ */

TEST_SUITE(ifconf);

TEST(ifconf_layout) {
	struct ifconf ifc;
	memset(&ifc, 0, sizeof(ifc));
	ASSERT_OFFSET(struct ifconf, ifc_len, 0);
	ASSERT_SIZE_SAME(ifc.ifc_len, int);
	/* Union starts after int + padding, aligned to pointer boundary */
	ASSERT_OFFSET_PAST(struct ifconf, ifc_ifcu, sizeof(int));
	ASSERT_OFFSET_ALIGNS(struct ifconf, ifc_ifcu, void*);
}

/* ============================================================================ */

TEST_SUITE(ifmap);

TEST(ifmap_layout) {
	struct ifmap map;
	memset(&map, 0, sizeof(map));
	ASSERT_OFFSET(struct ifmap, mem_start, 0);
	ASSERT_SIZE_SAME(map.mem_start, unsigned long);
}

TEST(ifmap_max_values) {
	struct ifmap m = {
		.mem_start = ULONG_MAX,
		.mem_end = ULONG_MAX,
		.base_addr = USHRT_MAX,
		.irq = UCHAR_MAX,
		.dma = UCHAR_MAX,
		.port = UCHAR_MAX
	};
	ASSERT_EQ(ULONG_MAX, m.mem_start);
	ASSERT_EQ(UCHAR_MAX, m.irq);
}

/* ============================================================================ */

TEST_SUITE(ifnameindex);

TEST(if_nameindex_layout) {
	struct if_nameindex idx;
	ASSERT_OFFSET(struct if_nameindex, if_index, 0);
	ASSERT_SIZE_SAME(idx.if_index, unsigned int);
	ASSERT_SIZE_SAME(idx.if_name, char*);
}

TEST(if_nameindex_empty_sentinel) {
	struct if_nameindex *list = if_nameindex();
	if (!list) TEST_SKIP("No interfaces or ENOMEM");
	/* Must terminate with {0, NULL} */
	int found = 0;
	for (int i = 0; i < 1000; i++) {
		if (list[i].if_index == 0 && list[i].if_name == NULL) { found = 1; break; }
	}
	ASSERT_TRUE(found);
	if_freenameindex(list);
}

/* ============================================================================ */

TEST_SUITE(if_nametoindex);

TEST(if_nametoindex_null) {
	errno = 0;
	ASSERT_EQ(0, if_nametoindex(NULL));
	/* POSIX: may or may not set errno on NULL, we just verify no crash */
}

TEST(if_nametoindex_empty_name) {
	ASSERT_EQ(0, if_nametoindex(""));
}

TEST(if_nametoindex_max_length_name) {
	char name[IFNAMSIZ] = {0};
	memset(name, 'a', IFNAMSIZ - 1);
	ASSERT_TRUE(if_nametoindex(name) == 0 || if_nametoindex(name) > 0); /* May exist or not */
}

TEST(if_nametoindex_nonexistent) {
	ASSERT_EQ(0, if_nametoindex("jacl_nonexistent_iface_12345"));
}

/* ============================================================================ */

TEST_SUITE(if_indextoname);

TEST(if_indextoname_null_buf) {
	ASSERT_NULL(if_indextoname(1, NULL));
}

TEST(if_indextoname_invalid_index) {
	char buf[IFNAMSIZ] = {0};
	char *ret = if_indextoname(0, buf); /* Index 0 is invalid */
	ASSERT_NULL(ret);
	ASSERT_TRUE(errno == ENOENT || errno == EINVAL);
}

TEST(if_indextoname_exact_buffer) {
	char buf[IFNAMSIZ] = {0};
	char *ret = if_indextoname(1, buf); /* Index 1 may or may not exist */
	if (ret) ASSERT_STR_PRE("", buf); /* If it returns, it's null-terminated */
}

TEST(if_indextoname_real_roundtrip) {
	struct if_nameindex *list = if_nameindex();
	if (!list) TEST_SKIP("No network interfaces available or ENOMEM");

	/* Find first valid interface */
	unsigned int target_idx = 0;
	char *target_name = NULL;
	for (int i = 0; list[i].if_index != 0; i++) {
		if (list[i].if_index > 0) {
			target_idx = list[i].if_index;
			target_name = list[i].if_name;
			break;
		}
	}

	if_freenameindex(list);
	if (!target_name) TEST_SKIP("No valid interfaces found in list");

	/* Roundtrip: name -> index -> name */
	unsigned int resolved_idx = if_nametoindex(target_name);
	ASSERT_EQ(target_idx, resolved_idx);

	char buf[IFNAMSIZ] = {0};
	char *ret = if_indextoname(target_idx, buf);
	ASSERT_NOT_NULL(ret);
	ASSERT_STR_EQ(target_name, buf);
}

/* ============================================================================ */

TEST_SUITE(if_freenameindex);

TEST(if_freenameindex_null_safe) {
	if_freenameindex(NULL);
	ASSERT_TRUE(1);
}

TEST(if_freenameindex_alloc_free) {
	struct if_nameindex *list = if_nameindex();
	if (!list) TEST_SKIP("Allocation failed or no interfaces");

	/* Verify termination sentinel */
	int found_sentinel = 0;
	for (int i = 0; i < 1000; i++) {
		if (list[i].if_index == 0 && list[i].if_name == NULL) {
			found_sentinel = 1;
			break;
		}
	}
	ASSERT_TRUE(found_sentinel);

	if_freenameindex(list); /* Should free safely */
	ASSERT_TRUE(1);
}

TEST_MAIN()

#else

int main(void) {
	printf("net/if.h requires POSIX\n");
	return 0;
}

#endif
