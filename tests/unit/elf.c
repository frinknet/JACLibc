/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <elf.h>
#include <string.h>
#include <stddef.h>

TEST_TYPE(unit);
TEST_UNIT(elf.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_elfmag) {
	ASSERT_STR_EQ(ELFMAG, "\177ELF");
}

TEST(constants_elfclass) {
	ASSERT_EQ(1, ELFCLASS32);
	ASSERT_EQ(2, ELFCLASS64);
}

TEST(constants_elfdata) {
	ASSERT_EQ(1, ELFDATA2LSB);
	ASSERT_EQ(2, ELFDATA2MSB);
}

TEST(constants_elftypes) {
	ASSERT_EQ(2, ET_EXEC);
	ASSERT_EQ(3, ET_DYN);
	ASSERT_EQ(4, ET_CORE);
}

TEST(constants_progtype) {
	ASSERT_EQ(1, PT_LOAD);
	ASSERT_EQ(2, PT_DYNAMIC);
	ASSERT_EQ(3, PT_INTERP);
	ASSERT_EQ(4, PT_NOTE);
}

TEST(constants_section_types) {
	ASSERT_EQ(0, SHT_NULL);
	ASSERT_EQ(1, SHT_PROGBITS);
	ASSERT_EQ(2, SHT_SYMTAB);
	ASSERT_EQ(3, SHT_STRTAB);
	ASSERT_EQ(6, SHT_DYNAMIC);
}

TEST(constants_symbol_bindings) {
	ASSERT_EQ(0, STB_LOCAL);
	ASSERT_EQ(1, STB_GLOBAL);
	ASSERT_EQ(2, STB_WEAK);
}

TEST(constants_symbol_types) {
	ASSERT_EQ(0, STT_NOTYPE);
	ASSERT_EQ(1, STT_OBJECT);
	ASSERT_EQ(2, STT_FUNC);
	ASSERT_EQ(3, STT_SECTION);
	ASSERT_EQ(4, STT_FILE);
}

TEST(constants_dynamic_tags) {
	ASSERT_EQ(0, DT_NULL);
	ASSERT_EQ(1, DT_NEEDED);
	ASSERT_EQ(5, DT_STRTAB);
	ASSERT_EQ(6, DT_SYMTAB);
	ASSERT_EQ(12, DT_INIT);
	ASSERT_EQ(13, DT_FINI);
}

/* ============================================================================ */
TEST_SUITE(Elf32_sizes);

TEST(Elf32_Ehdr_size) {
	ASSERT_EQ(52, sizeof(Elf32_Ehdr));
}

TEST(Elf32_Phdr_size) {
	ASSERT_EQ(32, sizeof(Elf32_Phdr));
}

TEST(Elf32_Shdr_size) {
	ASSERT_EQ(40, sizeof(Elf32_Shdr));
}

TEST(Elf32_Sym_size) {
	ASSERT_EQ(16, sizeof(Elf32_Sym));
}

TEST(Elf32_Rel_size) {
	ASSERT_EQ(8, sizeof(Elf32_Rel));
}

TEST(Elf32_Rela_size) {
	ASSERT_EQ(12, sizeof(Elf32_Rela));
}

/* ============================================================================ */
TEST_SUITE(Elf32_offsets);

TEST(Elf32_Ehdr_offset_e_type) {
	ASSERT_EQ(16, offsetof(Elf32_Ehdr, e_type));
}

TEST(Elf32_Ehdr_offset_e_entry) {
	ASSERT_EQ(24, offsetof(Elf32_Ehdr, e_entry));
}

TEST(Elf32_Ehdr_offset_e_phoff) {
	ASSERT_EQ(28, offsetof(Elf32_Ehdr, e_phoff));
}

TEST(Elf32_Ehdr_offset_e_shoff) {
	ASSERT_EQ(32, offsetof(Elf32_Ehdr, e_shoff));
}

/* ============================================================================ */
TEST_SUITE(macros_symbol_info);

TEST(macros_elf32_st_bind) {
	unsigned char info = 0x12; /* Global Func */
	ASSERT_EQ(1, ELF32_ST_BIND(info));
	ASSERT_EQ(2, ELF32_ST_TYPE(info));
}

TEST(macros_elf32_st_info) {
	unsigned char info = ELF32_ST_INFO(STB_GLOBAL, STT_FUNC);
	ASSERT_EQ(0x12, info);
}

TEST(macros_elf64_st_bind) {
	unsigned char info = 0x12;
	ASSERT_EQ(1, ELF64_ST_BIND(info));
	ASSERT_EQ(2, ELF64_ST_TYPE(info));
}

/* ============================================================================ */
TEST_SUITE(macros_relocation);

TEST(macros_elf32_r_sym) {
	Elf32_Word info = 0x501; /* Sym 5, Type 1 */
	ASSERT_EQ(5, ELF32_R_SYM(info));
	ASSERT_EQ(1, ELF32_R_TYPE(info));
}

TEST(macros_elf32_r_info) {
	Elf32_Word info = ELF32_R_INFO(5, 1);
	ASSERT_EQ(0x501, info);
}

TEST(macros_elf64_r_sym) {
	Elf64_Xword info = ((Elf64_Xword)0x100 << 32) | 0x2;
	ASSERT_EQ(0x100, ELF64_R_SYM(info));
	ASSERT_EQ(0x2, ELF64_R_TYPE(info));
}

TEST(macros_elf64_r_info) {
	Elf64_Xword info = ELF64_R_INFO(0x100, 0x2);
	ASSERT_EQ(((Elf64_Xword)0x100 << 32) | 0x2, info);
}

/* ============================================================================ */
TEST_SUITE(macros_dynamic_access);

TEST(macros_elf32_d_tag) {
	Elf32_Dyn dyn;
	dyn.d_tag = DT_NULL;
	ASSERT_EQ(DT_NULL, ELF32_D_TAG(dyn));
}

TEST(macros_elf32_d_val) {
	Elf32_Dyn dyn;
	dyn.d_un.d_val = 42;
	ASSERT_EQ(42, ELF32_D_VAL(dyn));
}

TEST(macros_elf32_d_ptr) {
	Elf32_Dyn dyn;
	dyn.d_un.d_ptr = 0x1000;
	ASSERT_EQ(0x1000, ELF32_D_PTR(dyn));
}

TEST(macros_elf64_d_tag) {
	Elf64_Dyn dyn;
	dyn.d_tag = DT_NEEDED;
	ASSERT_EQ(DT_NEEDED, ELF64_D_TAG(dyn));
}

TEST(macros_elf64_d_val) {
	Elf64_Dyn dyn;
	dyn.d_un.d_val = 12345;
	ASSERT_EQ(12345, ELF64_D_VAL(dyn));
}

TEST(macros_elf64_d_ptr) {
	Elf64_Dyn dyn;
	dyn.d_un.d_ptr = 0x2000;
	ASSERT_EQ(0x2000, ELF64_D_PTR(dyn));
}

/* ============================================================================ */
TEST_SUITE(macros_visibility);

TEST(macros_elf32_st_visibility) {
	ASSERT_EQ(0, ELF32_ST_VISIBILITY(0)); /* DEFAULT */
	ASSERT_EQ(1, ELF32_ST_VISIBILITY(1)); /* INTERNAL */
	ASSERT_EQ(2, ELF32_ST_VISIBILITY(2)); /* HIDDEN */
	ASSERT_EQ(3, ELF32_ST_VISIBILITY(3)); /* PROTECTED */
	/* Masking test: 0xFF & 0x3 == 3 */
	ASSERT_EQ(3, ELF32_ST_VISIBILITY(0xFF));
}

TEST(macros_elf64_st_visibility) {
	ASSERT_EQ(2, ELF64_ST_VISIBILITY(2));
}

/* ============================================================================ */
TEST_SUITE(macros_data_endianness);

TEST(macros_elf_data_macro) {
	Elf32_Ehdr hdr;
	/* Test LSB */
	hdr.e_ident[EI_DATA] = ELFDATA2LSB;
	ASSERT_EQ(ELFDATA2LSB, ELF_DATA(hdr));

	/* Test MSB */
	hdr.e_ident[EI_DATA] = ELFDATA2MSB;
	ASSERT_EQ(ELFDATA2MSB, ELF_DATA(hdr));
}

TEST(macros_elf_rd_compile_check) {
	Elf32_Ehdr hdr;
	hdr.e_ident[EI_DATA] = ELFDATA2LSB;
	hdr.e_type = 0x0102; /* LSB: 0x0201, MSB: 0x0102 */

	/* This just ensures the macro expands correctly with the struct member */
	volatile Elf32_Half val = ELF_RD16(hdr, e_type);
	ASSERT_TRUE(val == 0x0201 || val == 0x0102);
}

/* ============================================================================ */
#if JACL_BITS == 64

TEST_SUITE(Elf64_sizes);

TEST(Elf64_Ehdr_size) {
	ASSERT_EQ(64, sizeof(Elf64_Ehdr));
}

TEST(Elf64_Phdr_size) {
	ASSERT_EQ(56, sizeof(Elf64_Phdr));
}

TEST(Elf64_Shdr_size) {
	ASSERT_EQ(64, sizeof(Elf64_Shdr));
}

TEST(Elf64_Sym_size) {
	ASSERT_EQ(24, sizeof(Elf64_Sym));
}

TEST(Elf64_Rel_size) {
	ASSERT_EQ(16, sizeof(Elf64_Rel));
}

TEST(Elf64_Rela_size) {
	ASSERT_EQ(24, sizeof(Elf64_Rela));
}

/* ============================================================================ */
TEST_SUITE(Elf64_offsets);

TEST(Elf64_Ehdr_offset_e_type) {
	ASSERT_EQ(16, offsetof(Elf64_Ehdr, e_type));
}

TEST(Elf64_Ehdr_offset_e_entry) {
	ASSERT_EQ(24, offsetof(Elf64_Ehdr, e_entry));
}

TEST(Elf64_Ehdr_offset_e_phoff) {
	ASSERT_EQ(32, offsetof(Elf64_Ehdr, e_phoff));
}

TEST(Elf64_Ehdr_offset_e_shoff) {
	ASSERT_EQ(40, offsetof(Elf64_Ehdr, e_shoff));
}

#endif

/* ============================================================================ */
TEST_MAIN()
