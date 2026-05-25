/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _ARCH_MIPSN32_H
#define _ARCH_MIPSN32_H
#pragma once

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef mipsn32
	#define JACL_ARCH mipsn32
	#define JACL_ARCH_MIPSN32 1
	#define __jacl_arch_syscall __mipsn32_syscall
	#define __jacl_arch_tls_set __mipsn32_set_tp_register
	#define __jacl_arch_tls_get __mipsn32_get_tp_register
	#define __jacl_arch_clone_thread __mipsn32_clone_thread
	#define __jacl_arch_setjmp        __mipsn32_setjmp
	#define __jacl_arch_longjmp       __mipsn32_longjmp
	#define __jacl_arch_jmpbuf        __mipsn32_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __mipsn32_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long v0 __asm__("$2") = num;
		register long a0 __asm__("$4") = a1;
		register long a1_reg __asm__("$5") = a2;
		register long a2_reg __asm__("$6") = a3;
		register long a3_reg __asm__("$7") = a4;
		register long a4_reg __asm__("$8") = a5;
		register long a5_reg __asm__("$9") = a6;

		__asm__ volatile ("syscall"
			: "=r"(v0)
			: "r"(v0), "r"(a0), "r"(a1_reg), "r"(a2_reg), "r"(a3_reg), "r"(a4_reg), "r"(a5_reg)
			: "memory", "$1", "$3", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");

		result = v0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"move $fp, $0\n"
		"move $a0, $sp\n"
		"and $sp, $sp, -16\n"
		"jal _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			".set noreorder\n"
			"li $v0, 6211\n"
			"syscall\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __mipsn32_set_tp_register(void* addr) {
		__asm__ volatile("move $3, %0" : : "r"(addr) : "memory");
	}

	static inline void* __mipsn32_get_tp_register(void) {
		void* result;

		__asm__("move %0, $3" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __mipsn32_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"move $a0, %2\n\t"
			"move $a1, %3\n\t"
			"move $a2, $zero\n\t"
			"move $a3, $zero\n\t"
			"move $a4, $zero\n\t"
			"li $v0, 6055\n\t"
			"syscall\n\t"
			"bnez $v0, 1f\n\t"

			"move $fp, $zero\n\t"
			"move $a0, %5\n\t"
			"jalr %4\n\t"
			"li $v0, 6058\n\t"
			"syscall\n\t"

			"1:\n\t"
			"move %0, $v0\n\t"
			: "=r"(ret)
			: "r"((long)6055), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "$v0", "$a0", "$a1", "$a2", "$a3", "$a4", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long long __mipsn32_jmpbuf[20];

__asm__(
	".set noreorder\n"
	".text\n"
	".weak __mipsn32_setjmp\n"
	".type __mipsn32_setjmp, @function\n"
	"__mipsn32_setjmp:\n"
	"sd $ra, 0($4)\n"
	"sd $sp, 8($4)\n"
	"sd $gp, 16($4)\n"
	"sd $16, 24($4)\n"
	"sd $17, 32($4)\n"
	"sd $18, 40($4)\n"
	"sd $19, 48($4)\n"
	"sd $20, 56($4)\n"
	"sd $21, 64($4)\n"
	"sd $22, 72($4)\n"
	"sd $23, 80($4)\n"
	"sd $30, 88($4)\n"
#ifndef __mips_soft_float
	"sdc1 $24, 96($4)\n"
	"sdc1 $25, 104($4)\n"
	"sdc1 $26, 112($4)\n"
	"sdc1 $27, 120($4)\n"
	"sdc1 $28, 128($4)\n"
	"sdc1 $29, 136($4)\n"
	"sdc1 $30, 144($4)\n"
	"sdc1 $31, 152($4)\n"
#endif
	"jr $ra\n"
	"li $2, 0\n"
	".size __mipsn32_setjmp, .-__mipsn32_setjmp\n"
);

__asm__(
	".set noreorder\n"
	".text\n"
	".weak __mipsn32_longjmp\n"
	".type __mipsn32_longjmp, @function\n"
	"__mipsn32_longjmp:\n"
	"move $2, $5\n"
	"bne $2, $0, 1f\n"
	"nop\n"
	"addu $2, $2, 1\n"
	"1:\n"
#ifndef __mips_soft_float
	"ldc1 $24, 96($4)\n"
	"ldc1 $25, 104($4)\n"
	"ldc1 $26, 112($4)\n"
	"ldc1 $27, 120($4)\n"
	"ldc1 $28, 128($4)\n"
	"ldc1 $29, 136($4)\n"
	"ldc1 $30, 144($4)\n"
	"ldc1 $31, 152($4)\n"
#endif
	"ld $ra, 0($4)\n"
	"ld $sp, 8($4)\n"
	"ld $gp, 16($4)\n"
	"ld $16, 24($4)\n"
	"ld $17, 32($4)\n"
	"ld $18, 40($4)\n"
	"ld $19, 48($4)\n"
	"ld $20, 56($4)\n"
	"ld $21, 64($4)\n"
	"ld $22, 72($4)\n"
	"ld $23, 80($4)\n"
	"ld $30, 88($4)\n"
	"jr $ra\n"
	"nop\n"
	".size __mipsn32_longjmp, .-__mipsn32_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif

#endif /* _ARCH_MIPSN32_H */
