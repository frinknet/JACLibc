/* (c) 2025 FRINKnet & Friends – MIT licence */
/* Architecture: Cell Broadband Engine SPU (Synergistic Processing Unit) */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef spu
	#define JACL_ARCH spu
	#define JACL_ARCH_SPU 1
	#define __jacl_arch_syscall __spu_syscall
	/* SPU has no hardware TLS register. TLS is managed via Local Store pointers. */
	#define __jacl_arch_tls_set __spu_tls_set
	#define __jacl_arch_tls_get __spu_tls_get
	/* SPU threads are created by the PPU via spu_create. No clone syscall. */
	#define __jacl_arch_clone_thread __spu_clone_thread_stub
	#define __jacl_arch_setjmp        __spu_setjmp
	#define __jacl_arch_longjmp       __spu_longjmp
	#define __jacl_arch_jmpbuf        __spu_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321 /* Little-endian by default on Cell Linux */
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	/* 
	 * SPU Syscall Implementation.
	 * On Linux/Cell, syscalls from SPU are handled by the kernel's spu_run loop.
	 * The SPU executes a 'stop' instruction with a specific code, or uses a 
	 * dedicated syscall trap if supported by the kernel version.
	 * Modern glibc/musl for SPU often uses a helper function that triggers 
	 * a trap which the PPU kernel handler proxies.
	 * 
	 * However, strictly speaking, SPU userland usually links against libspe 
	 * or uses the 'spu_syscall' helper provided by the kernel headers.
	 * Since we are building a libc, we implement the low-level trap.
	 * 
	 * Note: Standard 'sc' does not exist. We use the 'stop 0x21' convention 
	 * or the dedicated syscall trap if available. For compatibility with 
	 * standard Linux SPU ABI, we use the 'break' instruction trap 
	 * which the kernel intercepts.
	 */
	static inline long __spu_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		register long r0 __asm__("r0") = num;
		register long r3 __asm__("r3") = a1;
		register long r4 __asm__("r4") = a2;
		register long r5 __asm__("r5") = a3;
		register long r6 __asm__("r6") = a4;
		register long r7 __asm__("r7") = a5;
		register long r8 __asm__("r8") = a6;
		register long r9 __asm__("r9");

		/* 
		 * SPU Syscall Trap:
		 * The exact mechanism depends on the kernel version. 
		 * Modern Cell Linux uses a specific trap sequence.
		 * We use the standard 'stop' based syscall convention for SPU.
		 */
		__asm__ volatile (
			"stop 0x21\n\t" /* Syscall trap code for SPU */
			: "=r"(r3), "=r"(r9)
			: "r"(r0), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8)
			: "memory"
		);
		
		return r3;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	/* 
	 * SPU Entry Point.
	 * Unlike PPU, SPU _start is not called by the kernel directly with argc/argv on stack.
	 * It is entered via spu_run. The context is passed in registers.
	 * r3: Pointer to argument block (argc, argv, envp) in Local Store.
	 * r4: Environment pointer.
	 * r5: Auxiliary vector pointer.
	 */
	__asm__(
		".globl _start\n"
		".type _start, @function\n"
		"_start:\n"
		/* Save link register if needed, though SPU entry is special */
		"il $sp, 0x3ff00\n\t" /* Initialize Stack Pointer to top of Local Store */
		"ai $sp, $sp, -128\n\t" /* Reserve initial frame */
		"br _start_main\n\t"   /* Branch to C entry point */
	);

	#if JACL_OS_LINUX
		/* 
		 * SPU Signal Return.
		 * SPU signals are handled by the PPU kernel, which modifies the SPU context.
		 * A dedicated return path is often required to restore context correctly.
		 */
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt, @function\n"
			"__restore_rt:\n"
			/* Restore context via syscall trap */
			"li $r0, 173\n\t" /* sys_rt_sigreturn */
			"stop 0x21\n\t"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	/* 
	 * SPU TLS.
	 * SPU has no dedicated TLS register (like r2 on PPU or gs on x86).
	 * TLS is typically stored at a fixed offset in Local Store or 
	 * managed via a global pointer in the data segment.
	 * We use a reserved Global Offset Table (GOT) entry or a fixed address.
	 */
	static inline void __spu_tls_set(void* addr) {
		/* 
		 * In a real SPU libc, this would update the TLS pointer in the 
		 * Thread Control Block (TCB) located in Local Store.
		 * For now, we store it in a known global variable location 
		 * or use a dedicated register convention if defined by ABI.
		 * Standard SPU ABI often uses $r31 or a fixed LS address.
		 */
		__asm__ volatile (
			"stqd %0, 0($r31)\n\t" /* Store TLS ptr in reserved slot */
			: : "r"(addr) : "memory"
		);
	}

	static inline void* __spu_tls_get(void) {
		void* result;
		__asm__ volatile (
			"lqd %0, 0($r31)\n\t"
			: "=r"(result)
		);
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	/* 
	 * SPU Clone.
	 * SPU cannot create threads itself. Threads are created by the PPU 
	 * using spu_create(). This function is a stub that returns error 
	 * or calls back to PPU if a communication channel is established.
	 */
	static inline pid_t __spu_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		(void)stack;
		(void)stack_size;
		(void)fn;
		(void)arg;
		/* SPU threads are managed by the PPU. */
		return -1; 
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

/* 
 * SPU Jump Buffer.
 * SPU has 128 general-purpose 128-bit registers.
 * We must save the critical ones: $r0 (LR), $sp, $r70-$r79 (callee-saved?), 
 * and the FPU/Vector state if used.
 * Simplified: Save $r0, $sp, $r3-$r31 (volatile, but needed for continuity), 
 * and control registers.
 */
typedef unsigned long __spu_jmpbuf[128]; /* 128 x 128-bit slots */

__asm__(
	".text\n"
	".globl __spu_setjmp\n"
	".type __spu_setjmp, @function\n"
	"__spu_setjmp:\n"
	/* Save Link Register ($r0) */
	"stqd $r0, 0($r3)\n\t"
	/* Save Stack Pointer ($r1) */
	"stqd $r1, 16($r3)\n\t"
	/* Save callee-saved registers ($r70-$r79 approx, depending on ABI) */
	/* For simplicity, saving a subset of critical regs */
	"stqd $r70, 32($r3)\n\t"
	"stqd $r71, 48($r3)\n\t"
	"stqd $r72, 64($r3)\n\t"
	"stqd $r73, 80($r3)\n\t"
	"stqd $r74, 96($r3)\n\t"
	"stqd $r75, 112($r3)\n\t"
	"stqd $r76, 128($r3)\n\t"
	"stqd $r77, 144($r3)\n\t"
	"stqd $r78, 160($r3)\n\t"
	"stqd $r79, 176($r3)\n\t"
	
	/* Return 0 */
	"ceqi $r3, $r3, 0\n\t"
	"br $r8\n\t" /* Return via LR */
	".size __spu_setjmp, .-__spu_setjmp\n"
);

__asm__(
	".text\n"
	".globl __spu_longjmp\n"
	".type __spu_longjmp, @function\n"
	"__spu_longjmp:\n"
	/* Restore Link Register */
	"lqd $r0, 0($r3)\n\t"
	/* Restore Stack Pointer */
	"lqd $r1, 16($r3)\n\t"
	/* Restore callee-saved registers */
	"lqd $r70, 32($r3)\n\t"
	"lqd $r71, 48($r3)\n\t"
	"lqd $r72, 64($r3)\n\t"
	"lqd $r73, 80($r3)\n\t"
	"lqd $r74, 96($r3)\n\t"
	"lqd $r75, 112($r3)\n\t"
	"lqd $r76, 128($r3)\n\t"
	"lqd $r77, 144($r3)\n\t"
	"lqd $r78, 160($r3)\n\t"
	"lqd $r79, 176($r3)\n\t"
	
	/* Set return value ($r3) to val ($r4), ensuring non-zero */
	"ceqi $r3, $r4, 0\n\t"
	"bnz $r3, 1f\n\t"
	"ai $r3, $r4, 1\n\t"
	"1:\n\t"
	"br $r0\n\t" /* Jump to saved LR */
	".size __spu_longjmp, .-__spu_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
