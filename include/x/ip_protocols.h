/**
 * PROTOCOL CONSTANTS  //  last updated: 2026-04-23 04:43:37 UTC
 * https://www.iana.org/assignments/protocol-numbers/protocol-numbers-1.csv
 */

X(IPPROTO_HOPOPT, 0, "HOPOPT", "IPv6 Hop-by-Hop Option")
X(IPPROTO_ICMP, 1, "ICMP", "Internet Control Message")
X(IPPROTO_IGMP, 2, "IGMP", "Internet Group Management")
X(IPPROTO_GGP, 3, "GGP", "Gateway-to-Gateway")
X(IPPROTO_IPV4, 4, "IPv4", "IPv4 encapsulation")
X(IPPROTO_ST, 5, "ST", "Stream")
X(IPPROTO_TCP, 6, "TCP", "Transmission Control")
X(IPPROTO_CBT, 7, "CBT", "CBT")
X(IPPROTO_EGP, 8, "EGP", "Exterior Gateway Protocol")
X(IPPROTO_IGP, 9, "IGP", "any private interior gateway")
X(IPPROTO_BBN_RCC_MON, 10, "BBN-RCC-MON", "BBN RCC Monitoring")
X(IPPROTO_NVP_II, 11, "NVP-II", "Network Voice Protocol")
X(IPPROTO_PUP, 12, "PUP", "PUP")
X(IPPROTO_ARGUS, 13, "ARGUS", "ARGUS")
X(IPPROTO_EMCON, 14, "EMCON", "EMCON")
X(IPPROTO_XNET, 15, "XNET", "Cross Net Debugger")
X(IPPROTO_CHAOS, 16, "CHAOS", "Chaos")
X(IPPROTO_UDP, 17, "UDP", "User Datagram")
X(IPPROTO_MUX, 18, "MUX", "Multiplexing")
X(IPPROTO_DCN_MEAS, 19, "DCN-MEAS", "DCN Measurement Subsystems")
X(IPPROTO_HMP, 20, "HMP", "Host Monitoring")
X(IPPROTO_PRM, 21, "PRM", "Packet Radio Measurement")
X(IPPROTO_XNS_IDP, 22, "XNS-IDP", "XEROX NS IDP")
X(IPPROTO_TRUNK_1, 23, "TRUNK-1", "Trunk-1")
X(IPPROTO_TRUNK_2, 24, "TRUNK-2", "Trunk-2")
X(IPPROTO_LEAF_1, 25, "LEAF-1", "Leaf-1")
X(IPPROTO_LEAF_2, 26, "LEAF-2", "Leaf-2")
X(IPPROTO_RDP, 27, "RDP", "Reliable Data Protocol")
X(IPPROTO_IRTP, 28, "IRTP", "Internet Reliable Transaction")
X(IPPROTO_ISO_TP4, 29, "ISO-TP4", "ISO Transport Protocol Class 4")
X(IPPROTO_NETBLT, 30, "NETBLT", "Bulk Data Transfer Protocol")
X(IPPROTO_MFE_NSP, 31, "MFE-NSP", "MFE Network Services Protocol")
X(IPPROTO_MERIT_INP, 32, "MERIT-INP", "MERIT Internodal Protocol")
X(IPPROTO_DCCP, 33, "DCCP", "Datagram Congestion Control Protocol")
X(IPPROTO_3PC, 34, "3PC", "Third Party Connect Protocol")
X(IPPROTO_IDPR, 35, "IDPR", "Inter-Domain Policy Routing Protocol")
X(IPPROTO_XTP, 36, "XTP", "XTP")
X(IPPROTO_DDP, 37, "DDP", "Datagram Delivery Protocol")
X(IPPROTO_IDPR_CMTP, 38, "IDPR-CMTP", "IDPR Control Message Transport Proto")
X(IPPROTO_TP, 39, "TP++", "TP++ Transport Protocol")
X(IPPROTO_IL, 40, "IL", "IL Transport Protocol")
X(IPPROTO_IPV6, 41, "IPv6", "IPv6 encapsulation")
X(IPPROTO_SDRP, 42, "SDRP", "Source Demand Routing Protocol")
X(IPPROTO_IPV6_ROUTE, 43, "IPv6-Route", "Routing Header for IPv6")
X(IPPROTO_IPV6_FRAG, 44, "IPv6-Frag", "Fragment Header for IPv6")
X(IPPROTO_IDRP, 45, "IDRP", "Inter-Domain Routing Protocol")
X(IPPROTO_RSVP, 46, "RSVP", "Reservation Protocol")
X(IPPROTO_GRE, 47, "GRE", "Generic Routing Encapsulation")
X(IPPROTO_DSR, 48, "DSR", "Dynamic Source Routing Protocol")
X(IPPROTO_BNA, 49, "BNA", "BNA")
X(IPPROTO_ESP, 50, "ESP", "Encap Security Payload")
X(IPPROTO_AH, 51, "AH", "Authentication Header")
X(IPPROTO_I_NLSP, 52, "I-NLSP", "Integrated Net Layer Security TUBA")
X(IPPROTO_SWIPE, 53, "SWIPE", "IP with Encryption")
X(IPPROTO_NARP, 54, "NARP", "NBMA Address Resolution Protocol")
X(IPPROTO_MIN_IPV4, 55, "Min-IPv4", "Minimal IPv4 Encapsulation")
X(IPPROTO_TLSP, 56, "TLSP", "Transport Layer Security Protocol")
X(IPPROTO_SKIP, 57, "SKIP", "SKIP")
X(IPPROTO_IPV6_ICMP, 58, "IPv6-ICMP", "ICMP for IPv6")
X(IPPROTO_IPV6_NONXT, 59, "IPv6-NoNxt", "No Next Header for IPv6")
X(IPPROTO_IPV6_OPTS, 60, "IPv6-Opts", "Destination Options for IPv6")
X(IPPROTO_PROTO_61, 61, "PROTO_61", "any host internal protocol")
X(IPPROTO_CFTP, 62, "CFTP", "CFTP")
X(IPPROTO_PROTO_63, 63, "PROTO_63", "any local network")
X(IPPROTO_SAT_EXPAK, 64, "SAT-EXPAK", "SATNET and Backroom EXPAK")
X(IPPROTO_KRYPTOLAN, 65, "KRYPTOLAN", "Kryptolan")
X(IPPROTO_RVD, 66, "RVD", "MIT Remote Virtual Disk Protocol")
X(IPPROTO_IPPC, 67, "IPPC", "Internet Pluribus Packet Core")
X(IPPROTO_PROTO_68, 68, "PROTO_68", "any distributed file system")
X(IPPROTO_SAT_MON, 69, "SAT-MON", "SATNET Monitoring")
X(IPPROTO_VISA, 70, "VISA", "VISA Protocol")
X(IPPROTO_IPCV, 71, "IPCV", "Internet Packet Core Utility")
X(IPPROTO_CPNX, 72, "CPNX", "Computer Protocol Network Executive")
X(IPPROTO_CPHB, 73, "CPHB", "Computer Protocol Heart Beat")
X(IPPROTO_WSN, 74, "WSN", "Wang Span Network")
X(IPPROTO_PVP, 75, "PVP", "Packet Video Protocol")
X(IPPROTO_BR_SAT_MON, 76, "BR-SAT-MON", "Backroom SATNET Monitoring")
X(IPPROTO_SUN_ND, 77, "SUN-ND", "SUN ND PROTOCOL-Temporary")
X(IPPROTO_WB_MON, 78, "WB-MON", "WIDEBAND Monitoring")
X(IPPROTO_WB_EXPAK, 79, "WB-EXPAK", "WIDEBAND EXPAK")
X(IPPROTO_ISO_IP, 80, "ISO-IP", "ISO Internet Protocol")
X(IPPROTO_VMTP, 81, "VMTP", "VMTP")
X(IPPROTO_SECURE_VMTP, 82, "SECURE-VMTP", "SECURE-VMTP")
X(IPPROTO_VINES, 83, "VINES", "VINES")
X(IPPROTO_IPTM, 84, "IPTM", "Internet Protocol Traffic Manager")
X(IPPROTO_NSFNET_IGP, 85, "NSFNET-IGP", "NSFNET-IGP")
X(IPPROTO_DGP, 86, "DGP", "Dissimilar Gateway Protocol")
X(IPPROTO_TCF, 87, "TCF", "TCF")
X(IPPROTO_EIGRP, 88, "EIGRP", "EIGRP")
X(IPPROTO_OSPFIGP, 89, "OSPFIGP", "OSPFIGP")
X(IPPROTO_SPRITE_RPC, 90, "Sprite-RPC", "Sprite RPC Protocol")
X(IPPROTO_LARP, 91, "LARP", "Locus Address Resolution Protocol")
X(IPPROTO_MTP, 92, "MTP", "Multicast Transport Protocol")
X(IPPROTO_AX_25, 93, "AX.25", "AX.25 Frames")
X(IPPROTO_IPIP, 94, "IPIP", "IP-within-IP Encapsulation Protocol")
X(IPPROTO_MICP, 95, "MICP", "Mobile Internetworking Control Pro.")
X(IPPROTO_SCC_SP, 96, "SCC-SP", "Semaphore Communications Sec. Pro.")
X(IPPROTO_ETHERIP, 97, "ETHERIP", "Ethernet-within-IP Encapsulation")
X(IPPROTO_ENCAP, 98, "ENCAP", "Encapsulation Header")
X(IPPROTO_PROTO_99, 99, "PROTO_99", "any private encryption scheme")
X(IPPROTO_GMTP, 100, "GMTP", "GMTP")
X(IPPROTO_IFMP, 101, "IFMP", "Ipsilon Flow Management Protocol")
X(IPPROTO_PNNI, 102, "PNNI", "PNNI over IP")
X(IPPROTO_PIM, 103, "PIM", "Protocol Independent Multicast")
X(IPPROTO_ARIS, 104, "ARIS", "ARIS")
X(IPPROTO_SCPS, 105, "SCPS", "SCPS")
X(IPPROTO_QNX, 106, "QNX", "QNX")
X(IPPROTO_A_N, 107, "A/N", "Active Networks")
X(IPPROTO_IPCOMP, 108, "IPComp", "IP Payload Compression Protocol")
X(IPPROTO_SNP, 109, "SNP", "Sitara Networks Protocol")
X(IPPROTO_COMPAQ_PEER, 110, "Compaq-Peer", "Compaq Peer Protocol")
X(IPPROTO_IPX_IN_IP, 111, "IPX-in-IP", "IPX in IP")
X(IPPROTO_VRRP, 112, "VRRP", "Virtual Router Redundancy Protocol")
X(IPPROTO_PGM, 113, "PGM", "PGM Reliable Transport Protocol")
X(IPPROTO_PROTO_114, 114, "PROTO_114", "any 0-hop protocol")
X(IPPROTO_L2TP, 115, "L2TP", "Layer Two Tunneling Protocol")
X(IPPROTO_DDX, 116, "DDX", "D-II Data Exchange (DDX)")
X(IPPROTO_IATP, 117, "IATP", "Interactive Agent Transfer Protocol")
X(IPPROTO_STP, 118, "STP", "Schedule Transfer Protocol")
X(IPPROTO_SRP, 119, "SRP", "SpectraLink Radio Protocol")
X(IPPROTO_UTI, 120, "UTI", "UTI")
X(IPPROTO_SMP, 121, "SMP", "Simple Message Protocol")
X(IPPROTO_SM, 122, "SM", "Simple Multicast Protocol")
X(IPPROTO_PTP, 123, "PTP", "Performance Transparency Protocol")
X(IPPROTO_ISIS_OVER_IPV4, 124, "ISIS over IPv4", "Unassigned")
X(IPPROTO_FIRE, 125, "FIRE", "Unassigned")
X(IPPROTO_CRTP, 126, "CRTP", "Combat Radio Transport Protocol")
X(IPPROTO_CRUDP, 127, "CRUDP", "Combat Radio User Datagram")
X(IPPROTO_SSCOPMCE, 128, "SSCOPMCE", "Unassigned")
X(IPPROTO_IPLT, 129, "IPLT", "Unassigned")
X(IPPROTO_SPS, 130, "SPS", "Secure Packet Shield")
X(IPPROTO_PIPE, 131, "PIPE", "Private IP Encapsulation within IP")
X(IPPROTO_SCTP, 132, "SCTP", "Stream Control Transmission Protocol")
X(IPPROTO_FC, 133, "FC", "Fibre Channel")
X(IPPROTO_RSVP_E2E_IGNORE, 134, "RSVP-E2E-IGNORE", "Unassigned")
X(IPPROTO_MOBILITY_HEADER, 135, "Mobility Header", "Unassigned")
X(IPPROTO_UDPLITE, 136, "UDPLite", "Unassigned")
X(IPPROTO_MPLS_IN_IP, 137, "MPLS-in-IP", "Unassigned")
X(IPPROTO_MANET, 138, "manet", "MANET Protocols")
X(IPPROTO_HIP, 139, "HIP", "Host Identity Protocol")
X(IPPROTO_SHIM6, 140, "Shim6", "Shim6 Protocol")
X(IPPROTO_WESP, 141, "WESP", "Wrapped Encapsulating Security Payload")
X(IPPROTO_ROHC, 142, "ROHC", "Robust Header Compression")
X(IPPROTO_ETHERNET, 143, "Ethernet", "Ethernet")
X(IPPROTO_AGGFRAG, 144, "AGGFRAG", "AGGFRAG encapsulation payload for ESP")
X(IPPROTO_NSH, 145, "NSH", "Network Service Header")
X(IPPROTO_HOMA, 146, "Homa", "Homa")
X(IPPROTO_BIT_EMU, 147, "BIT-EMU", "Bit-stream Emulation")
/* 148: Unassigned */
/* 149: Unassigned */
/* 150: Unassigned */
/* 151: Unassigned */
/* 152: Unassigned */
/* 153: Unassigned */
/* 154: Unassigned */
/* 155: Unassigned */
/* 156: Unassigned */
/* 157: Unassigned */
/* 158: Unassigned */
/* 159: Unassigned */
/* 160: Unassigned */
/* 161: Unassigned */
/* 162: Unassigned */
/* 163: Unassigned */
/* 164: Unassigned */
/* 165: Unassigned */
/* 166: Unassigned */
/* 167: Unassigned */
/* 168: Unassigned */
/* 169: Unassigned */
/* 170: Unassigned */
/* 171: Unassigned */
/* 172: Unassigned */
/* 173: Unassigned */
/* 174: Unassigned */
/* 175: Unassigned */
/* 176: Unassigned */
/* 177: Unassigned */
/* 178: Unassigned */
/* 179: Unassigned */
/* 180: Unassigned */
/* 181: Unassigned */
/* 182: Unassigned */
/* 183: Unassigned */
/* 184: Unassigned */
/* 185: Unassigned */
/* 186: Unassigned */
/* 187: Unassigned */
/* 188: Unassigned */
/* 189: Unassigned */
/* 190: Unassigned */
/* 191: Unassigned */
/* 192: Unassigned */
/* 193: Unassigned */
/* 194: Unassigned */
/* 195: Unassigned */
/* 196: Unassigned */
/* 197: Unassigned */
/* 198: Unassigned */
/* 199: Unassigned */
/* 200: Unassigned */
/* 201: Unassigned */
/* 202: Unassigned */
/* 203: Unassigned */
/* 204: Unassigned */
/* 205: Unassigned */
/* 206: Unassigned */
/* 207: Unassigned */
/* 208: Unassigned */
/* 209: Unassigned */
/* 210: Unassigned */
/* 211: Unassigned */
/* 212: Unassigned */
/* 213: Unassigned */
/* 214: Unassigned */
/* 215: Unassigned */
/* 216: Unassigned */
/* 217: Unassigned */
/* 218: Unassigned */
/* 219: Unassigned */
/* 220: Unassigned */
/* 221: Unassigned */
/* 222: Unassigned */
/* 223: Unassigned */
/* 224: Unassigned */
/* 225: Unassigned */
/* 226: Unassigned */
/* 227: Unassigned */
/* 228: Unassigned */
/* 229: Unassigned */
/* 230: Unassigned */
/* 231: Unassigned */
/* 232: Unassigned */
/* 233: Unassigned */
/* 234: Unassigned */
/* 235: Unassigned */
/* 236: Unassigned */
/* 237: Unassigned */
/* 238: Unassigned */
/* 239: Unassigned */
/* 240: Unassigned */
/* 241: Unassigned */
/* 242: Unassigned */
/* 243: Unassigned */
/* 244: Unassigned */
/* 245: Unassigned */
/* 246: Unassigned */
/* 247: Unassigned */
/* 248: Unassigned */
/* 249: Unassigned */
/* 250: Unassigned */
/* 251: Unassigned */
/* 252: Unassigned */
X(IPPROTO_EXP1, 253, "EXP1", "Use for experimentation and testing")
X(IPPROTO_EXP2, 254, "EXP2", "Use for experimentation and testing")
X(IPPROTO_RESERVED, 255, "Reserved", "Reserved")
