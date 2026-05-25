/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef powerpc32
	#define JACL_ARCH powerpc32
	#define JACL_ARCH_POWERPC32 1
	#define __jacl_arch_syscall __powerpc32_syscall
	#define __jacl_arch_tls_set __powerpc32_set_tp_register
	#define __jacl_arch_tls_get __powerpc32_get_tp_register
	#define __jacl_arch_clone_thread __powerpc32_clone_thread
	#define __jacl_arch_setjmp        __powerpc32_setjmp
	#define __jacl_arch_longjmp       __powerpc32_longjmp
	#define __jacl_arch_jmpbuf        __powerpc32_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __powerpc32_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r0 __asm__("r0") = num;
		register long r3 __asm__("r3") = a1;
		register long r4 __asm__("r4") = a2;
		register long r5 __asm__("r5") = a3;
		register long r6 __asm__("r6") = a4;
		register long r7 __asm__("r7") = a5;
		register long r8 __asm__("r8") = a6;

		__asm__ volatile ("sc"
			: "=r"(r3)
			: "r"(r0), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8)
			: "memory", "cr0", "r9", "r10", "r11", "r12");

		result = r3;
		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"li 31, 0\n"
		"mr 3, 1\n"
		"clrrwi 1, 1, 4\n"
		"bl _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"li 0, 173\n"
			"sc\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __powerpc32_set_tp_register(void* addr) {
		__asm__ volatile("mr 2, %0" : : "r"(addr) : "memory");
	}

	static inline void* __powerpc32_get_tp_register(void) {
		void* result;

		__asm__("mr %0, 2" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __powerpc32_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 112;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"mr 3, %2\n\t"
			"mr 4, %3\n\t"
			"li 5, 0\n\t"
			"li 6, 0\n\t"
			"li 7, 0\n\t"
			"li 0, 120\n\t"
			"sc\n\t"
			"cmpwi 3, 0\n\t"
			"bne 1f\n\t"

			"li 31, 0\n\t"
			"mr 3, %5\n\t"
			"mtctr %4\n\t"
			"bctrl\n\t"
			"li 0, 1\n\t"
			"sc\n\t"

			"1:\n\t"
			"mr %0, 3\n\t"
			: "=r"(ret)
			: "r"((long)120), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r0", "r3", "r4", "r5", "r6", "r7", "ctr", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __powerpc32_jmpbuf[64];

/* Weak fallback to prevent link errors if hwcap parser is not yet implemented */
__attribute__((weak)) unsigned long __hwcap = 0;

__asm__(
	".text\n"
	".weak __powerpc32_setjmp\n"
	".type __powerpc32_setjmp, @function\n"
	"__powerpc32_setjmp:\n"
	"mflr 0\n"
	"stw 0, 0(3)\n"
	"stw 1, 4(3)\n"
	"mfcr 0\n"
	"stw 0, 8(3)\n"
	"stw 14, 12(3)\n"
	"stw 15, 16(3)\n"
	"stw 16, 20(3)\n"
	"stw 17, 24(3)\n"
	"stw 18, 28(3)\n"
	"stw 19, 32(3)\n"
	"stw 20, 36(3)\n"
	"stw 21, 40(3)\n"
	"stw 22, 44(3)\n"
	"stw 23, 48(3)\n"
	"stw 24, 52(3)\n"
	"stw 25, 56(3)\n"
	"stw 26, 60(3)\n"
	"stw 27, 64(3)\n"
	"stw 28, 68(3)\n"
	"stw 29, 72(3)\n"
	"stw 30, 76(3)\n"
	"stw 31, 80(3)\n"
#if defined(_SOFT_FLOAT) || defined(__NO_FPRS__)
	"mflr 0\n"
	"bl 1f\n"
	".hidden __hwcap\n"
	".long __hwcap-.\n"
	"1:\n"
	"mflr 4\n"
	"lwz 5, 0(4)\n"
	"lwzx 4, 4, 5\n"
	"andis. 4, 4, 0x80\n"
	"beq 1f\n"
	".long 0x11c35b21\n"
	".long 0x11e36321\n"
	".long 0x12036b21\n"
	".long 0x12237321\n"
	".long 0x12437b21\n"
	".long 0x12638321\n"
	".long 0x12838b21\n"
	".long 0x12a39321\n"
	".long 0x12c39b21\n"
	".long 0x12e3a321\n"
	".long 0x1303ab21\n"
	".long 0x1323b321\n"
	".long 0x1343bb21\n"
	".long 0x1363c321\n"
	".long 0x1383cb21\n"
	".long 0x13a3d321\n"
	".long 0x13c3db21\n"
	".long 0x13e3e321\n"
	".long 0x11a3eb21\n"
	"1:\n"
	"mtlr 0\n"
#else
	"stfd 14,88(3)\n"
	"stfd 15,96(3)\n"
	"stfd 16,104(3)\n"
	"stfd 17,112(3)\n"
	"stfd 18,120(3)\n"
	"stfd 19,128(3)\n"
	"stfd 20,136(3)\n"
	"stfd 21,144(3)\n"
	"stfd 22,152(3)\n"
	"stfd 23,160(3)\n"
	"stfd 24,168(3)\n"
	"stfd 25,176(3)\n"
	"stfd 26,184(3)\n"
	"stfd 27,192(3)\n"
	"stfd 28,200(3)\n"
	"stfd 29,208(3)\n"
	"stfd 30,216(3)\n"
	"stfd 31,224(3)\n"
#endif
	"li 3, 0\n"
	"blr\n"
	".size __powerpc32_setjmp, .-__powerpc32_setjmp\n"
);

__asm__(
	".text\n"
	".weak __powerpc32_longjmp\n"
	".type __powerpc32_longjmp, @function\n"
	"__powerpc32_longjmp:\n"
	"lwz 0, 0(3)\n"
	"mtlr 0\n"
	"lwz 1, 4(3)\n"
	"lwz 0, 8(3)\n"
	"mtcr 0\n"
	"lwz 14, 12(3)\n"
	"lwz 15, 16(3)\n"
	"lwz 16, 20(3)\n"
	"lwz 17, 24(3)\n"
	"lwz 18, 28(3)\n"
	"lwz 19, 32(3)\n"
	"lwz 20, 36(3)\n"
	"lwz 21, 40(3)\n"
	"lwz 22, 44(3)\n"
	"lwz 23, 48(3)\n"
	"lwz 24, 52(3)\n"
	"lwz 25, 56(3)\n"
	"lwz 26, 60(3)\n"
	"lwz 27, 64(3)\n"
	"lwz 28, 68(3)\n"
	"lwz 29, 72(3)\n"
	"lwz 30, 76(3)\n"
	"lwz 31, 80(3)\n"
#if defined(_SOFT_FLOAT) || defined(__NO_FPRS__)
	"mflr 0\n"
	"bl 1f\n"
	".hidden __hwcap\n"
	".long __hwcap-.\n"
	"1:\n"
	"mflr 6\n"
	"lwz 5, 0(6)\n"
	"lwzx 6, 6, 5\n"
	"andis. 6, 6, 0x80\n"
	"beq 1f\n"
	".long 0x11c35b01\n"
	".long 0x11e36301\n"
	".long 0x12036b01\n"
	".long 0x12237301\n"
	".long 0x12437b01\n"
	".long 0x12638301\n"
	".long 0x12838b01\n"
	".long 0x12a39301\n"
	".long 0x12c39b01\n"
	".long 0x12e3a301\n"
	".long 0x1303ab01\n"
	".long 0x1323b301\n"
	".long 0x1343bb01\n"
	".long 0x1363c301\n"
	".long 0x1383cb01\n"
	".long 0x13a3d301\n"
	".long 0x13c3db01\n"
	".long 0x13e3e301\n"
	".long 0x11a3eb01\n"
	"1:\n"
	"mtlr 0\n"
#else
	"lfd 14,88(3)\n"
	"lfd 15,96(3)\n"
	"lfd 16,104(3)\n"
	"lfd 17,112(3)\n"
	"lfd 18,120(3)\n"
	"lfd 19,128(3)\n"
	"lfd 20,136(3)\n"
	"lfd 21,144(3)\n"
	"lfd 22,152(3)\n"
	"lfd 23,160(3)\n"
	"lfd 24,168(3)\n"
	"lfd 25,176(3)\n"
	"lfd 26,184(3)\n"
	"lfd 27,192(3)\n"
	"lfd 28,200(3)\n"
	"lfd 29,208(3)\n"
	"lfd 30,216(3)\n"
	"lfd 31,224(3)\n"
#endif
	"mr 3, 4\n"
	"cmpwi cr7, 4, 0\n"
	"bne cr7, 1f\n"
	"li 3, 1\n"
	"1:\n"
	"blr\n"
	".size __powerpc32_longjmp, .-__powerpc32_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
