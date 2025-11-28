/**
 * LINUX EM FORMAT IMPORTED  //  last updated: 2025-11-28 19:21:53 UTC
 * https://raw.githubusercontent.com/torvalds/linux/v6.17/include/uapi/linux/elf-em.h
 */

/* ELF Machine Types */
#define ELF_MACHINES(X) \
	X(EM_NONE, 0) \
	X(EM_M32, 1) \
	X(EM_SPARC, 2) \
	X(EM_386, 3) \
	X(EM_68K, 4) \
	X(EM_88K, 5) \
	X(EM_486, 6 /* Perhaps disused */) \
	X(EM_860, 7) \
	X(EM_MIPS, 8 /* MIPS R3000 (officially, big-endian only) */) \
	X(EM_MIPS_RS3_LE, 10 /* MIPS R3000 little-endian */) \
	X(EM_MIPS_RS4_BE, 10 /* MIPS R4000 big-endian */) \
	X(EM_PARISC, 15 /* HPPA */) \
	X(EM_SPARC32PLUS, 18 /* Sun's "v8plus" */) \
	X(EM_PPC, 20 /* PowerPC */) \
	X(EM_PPC64, 21 /* PowerPC64 */) \
	X(EM_SPU, 23 /* Cell BE SPU */) \
	X(EM_ARM, 40 /* ARM 32 bit */) \
	X(EM_SH, 42 /* SuperH */) \
	X(EM_SPARCV9, 43 /* SPARC v9 64-bit */) \
	X(EM_H8_300, 46 /* Renesas H8/300 */) \
	X(EM_IA_64, 50 /* HP/Intel IA-64 */) \
	X(EM_X86_64, 62 /* AMD x86-64 */) \
	X(EM_S390, 22 /* IBM S/390 */) \
	X(EM_CRIS, 76 /* Axis Communications 32-bit embedded processor */) \
	X(EM_M32R, 88 /* Renesas M32R */) \
	X(EM_MN10300, 89 /* Panasonic/MEI MN10300, AM33 */) \
	X(EM_OPENRISC, 92 /* OpenRISC 32-bit embedded processor */) \
	X(EM_ARCOMPACT, 93 /* ARCompact processor */) \
	X(EM_XTENSA, 94 /* Tensilica Xtensa Architecture */) \
	X(EM_BLACKFIN, 106 /* ADI Blackfin Processor */) \
	X(EM_UNICORE, 110 /* UniCore-32 */) \
	X(EM_ALTERA_NIOS2, 113 /* Altera Nios II soft-core processor */) \
	X(EM_TI_C6000, 140 /* TI C6X DSPs */) \
	X(EM_HEXAGON, 164 /* QUALCOMM Hexagon */) \
	X(EM_NDS32, 167 /* Andes Technology compact code size) \
	X(EM_AARCH64, 183 /* ARM 64 bit */) \
	X(EM_TILEPRO, 188 /* Tilera TILEPro */) \
	X(EM_MICROBLAZE, 189 /* Xilinx MicroBlaze */) \
	X(EM_TILEGX, 191 /* Tilera TILE-Gx */) \
	X(EM_ARCV2, 195 /* ARCv2 Cores */) \
	X(EM_RISCV, 243 /* RISC-V */) \
	X(EM_BPF, 247 /* Linux BPF - in-kernel virtual machine */) \
	X(EM_CSKY, 252 /* C-SKY */) \
	X(EM_LOONGARCH, 258 /* LoongArch */) \
	X(EM_FRV, 0x5441 /* Fujitsu FR-V */) \
	X(EM_ALPHA, 0x9026) \
	X(EM_CYGNUS_M32R, 0x9041) \
	X(EM_S390_OLD, 0xA390) \
	X(EM_CYGNUS_MN10300, 0xbeef)

