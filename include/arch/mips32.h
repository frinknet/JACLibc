/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef mips32
	#define JACL_ARCH mips32
	#define JACL_ARCH_MIPS32 1
	#define __jacl_arch_syscall __mips32_syscall
	#define __jacl_arch_tls_set __mips32_set_tp_register
	#define __jacl_arch_tls_get __mips32_get_tp_register
	#define __jacl_arch_clone_thread __mips32_clone_thread
	#define __jacl_arch_setjmp        __mips32_setjmp
	#define __jacl_arch_longjmp       __mips32_longjmp
	#define __jacl_arch_jmpbuf        __mips32_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __mips32_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long v0 __asm__("$2") = num;
		register long a0 __asm__("$4") = a1;
		register long a1_reg __asm__("$5") = a2;
		register long a2_reg __asm__("$6") = a3;
		register long a3_reg __asm__("$7") = a4;

		(void)a5;
		(void)a6;

		__asm__ volatile ("syscall"
			: "=r"(v0)
			: "r"(v0), "r"(a0), "r"(a1_reg), "r"(a2_reg), "r"(a3_reg)
			: "memory", "$1", "$3", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");

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
			"li $v0, 4193\n"
			"syscall\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __mips32_set_tp_register(void* addr) {
		__asm__ volatile("move $3, %0" : : "r"(addr) : "memory");
	}

	static inline void* __mips32_get_tp_register(void) {
		void* result;

		__asm__("move %0, $3" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __mips32_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 8;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"move $a0, %2\n\t"
			"move $a1, %3\n\t"
			"move $a2, $zero\n\t"
			"move $a3, $zero\n\t"
			"li $v0, 4120\n\t"
			"syscall\n\t"
			"bnez $v0, 1f\n\t"

			"move $fp, $zero\n\t"
			"move $a0, %5\n\t"
			"jalr %4\n\t"
			"li $v0, 4001\n\t"
			"syscall\n\t"

			"1:\n\t"
			"move %0, $v0\n\t"
			: "=r"(ret)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "$v0", "$a0", "$a1", "$a2", "$a3", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __mips32_jmpbuf[24];

__asm__(
	".set noreorder\n"
	".text\n"
	".weak __mips32_setjmp\n"
	".type __mips32_setjmp, @function\n"
	"__mips32_setjmp:\n"
	"sw $ra, 0($4)\n"
	"sw $sp, 4($4)\n"
	"sw $16, 8($4)\n"
	"sw $17, 12($4)\n"
	"sw $18, 16($4)\n"
	"sw $19, 20($4)\n"
	"sw $20, 24($4)\n"
	"sw $21, 28($4)\n"
	"sw $22, 32($4)\n"
	"sw $23, 36($4)\n"
	"sw $30, 40($4)\n"
	"sw $28, 44($4)\n"
#ifndef __mips_soft_float
	"s.d $f20, 56($4)\n"
	"s.d $f22, 64($4)\n"
	"s.d $f24, 72($4)\n"
	"s.d $f26, 80($4)\n"
	"s.d $f28, 88($4)\n"
	"s.d $f30, 96($4)\n"
#endif
	"jr $ra\n"
	"li $2, 0\n"
	".size __mips32_setjmp, .-__mips32_setjmp\n"
);

__asm__(
	".set noreorder\n"
	".text\n"
	".weak __mips32_longjmp\n"
	".type __mips32_longjmp, @function\n"
	"__mips32_longjmp:\n"
	"move $2, $5\n"
	"bne $2, $0, 1f\n"
	"nop\n"
	"addu $2, $2, 1\n"
	"1:\n"
#ifndef __mips_soft_float
	"l.d $f20, 56($4)\n"
	"l.d $f22, 64($4)\n"
	"l.d $f24, 72($4)\n"
	"l.d $f26, 80($4)\n"
	"l.d $f28, 88($4)\n"
	"l.d $f30, 96($4)\n"
#endif
	"lw $ra, 0($4)\n"
	"lw $sp, 4($4)\n"
	"lw $16, 8($4)\n"
	"lw $17, 12($4)\n"
	"lw $18, 16($4)\n"
	"lw $19, 20($4)\n"
	"lw $20, 24($4)\n"
	"lw $21, 28($4)\n"
	"lw $22, 32($4)\n"
	"lw $23, 36($4)\n"
	"lw $30, 40($4)\n"
	"jr $ra\n"
	"lw $28, 44($4)\n"
	".size __mips32_longjmp, .-__mips32_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
