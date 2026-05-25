/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef ia64
	#define JACL_ARCH ia64
	#define JACL_ARCH_IA64 1
	#define __jacl_arch_syscall __ia64_syscall
	#define __jacl_arch_tls_set __ia64_set_tp_register
	#define __jacl_arch_tls_get __ia64_get_tp_register
	#define __jacl_arch_clone_thread __ia64_clone_thread
	#define __jacl_arch_setjmp        __ia64_setjmp
	#define __jacl_arch_longjmp       __ia64_longjmp
	#define __jacl_arch_jmpbuf        __ia64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __ia64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r15 __asm__("r15") = num;
		register long out0 __asm__("out0") = a1;
		register long out1 __asm__("out1") = a2;
		register long out2 __asm__("out2") = a3;
		register long out3 __asm__("out3") = a4;
		register long out4 __asm__("out4") = a5;
		register long out5 __asm__("out5") = a6;

		__asm__ volatile ("break 0x100000"
			: "=r"(r8), "=r"(r10)
			: "r"(r15), "r"(out0), "r"(out1), "r"(out2), "r"(out3), "r"(out4), "r"(out5)
			: "memory");

		result = r8;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"{ .mlx\n"
		"alloc r2 = ar.pfs, 0, 0, 1, 0\n"
		"movl r32 = @gprel(_start_main)\n"
		";;\n"
		"}\n"
		"{ .mib\n"
		"add r12 = -16, r12\n"
		"and r12 = -16, r12\n"
		"br.call.sptk.many b0 = _start_main\n"
		";;\n"
		"}\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __ia64_set_tp_register(void* addr) {
		__asm__ volatile("mov r13 = %0" : : "r"(addr) : "memory");
	}

	static inline void* __ia64_get_tp_register(void) {
		void* result;

		__asm__("mov %0 = r13" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __ia64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r8 __asm__("r8");

		__asm__ volatile(
			"mov r32 = %1\n\t"
			"mov r33 = %2\n\t"
			"mov r34 = r0\n\t"
			"mov r35 = r0\n\t"
			"mov r36 = r0\n\t"
			"mov r15 = 1128\n\t"
			"break 0x100000\n\t"
			"cmp.ne p6, p7 = r8, r0\n\t"
			"(p6) br.cond.dptk 1f\n\t"

			"mov r32 = %4\n\t"
			"mov b6 = %3\n\t"
			"br.call.sptk.many b0 = b6\n\t"
			"mov r15 = 1025\n\t"
			"break 0x100000\n\t"

			"1:\n\t"
			: "=r"(r8)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r15", "r32", "r33", "r34", "r35", "r36", "b6", "memory"
		);

		return (pid_t)r8;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __ia64_jmpbuf[64];

__asm__(
	".text\n"
	".weak __ia64_setjmp\n"
	".type __ia64_setjmp, @function\n"
	"__ia64_setjmp:\n"
	"{ .mmi\n"
	"  alloc r2 = ar.pfs, 1, 0, 0, 0\n"
	"  mov r3 = ar.bsp\n"
	"  mov r4 = ar.rnat\n"
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r12, 8\n" /* sp */
	"  st8 [r32] = r3, 8\n"  /* bsp */
	"  st8 [r32] = r2, 8\n"  /* pfs */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r4, 8\n"  /* rnat */
	"  mov r4 = ar.unat\n"
	"  mov r5 = r1\n"        /* gp */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r4, 8\n"  /* unat */
	"  st8 [r32] = r5, 8\n"  /* gp */
	"  mov r5 = b0\n"        /* rp */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r5, 8\n"  /* b0 */
	"  mov r5 = b1\n"
	"  mov r6 = b2\n"
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r5, 8\n"  /* b1 */
	"  st8 [r32] = r6, 8\n"  /* b2 */
	"  mov r6 = b3\n"
	"}\n"
	"{ .mmi\n"
	"  mov r7 = b4\n"
	"  mov r8 = b5\n"
	"  mov r9 = pr\n"
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r6, 8\n"  /* b3 */
	"  st8 [r32] = r7, 8\n"  /* b4 */
	"  st8 [r32] = r8, 8\n"  /* b5 */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r9, 8\n"  /* pr */
	"  mov r9 = r4\n"        /* r4 */
	"  mov r10 = r5\n"       /* r5 */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r9, 8\n"  /* r4 */
	"  st8 [r32] = r10, 8\n" /* r5 */
	"  mov r10 = r6\n"       /* r6 */
	"}\n"
	"{ .mmi\n"
	"  mov r11 = r7\n"       /* r7 */
	"  mov r12 = r8\n"       /* r8 */
	"  mov r13 = r9\n"       /* r9 */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r10, 8\n" /* r6 */
	"  st8 [r32] = r11, 8\n" /* r7 */
	"  st8 [r32] = r12, 8\n" /* r8 */
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r13, 8\n" /* r9 */
	"  mov r13 = r10\n"      /* r10 */
	"  nop.m 0\n"
	"}\n"
	"{ .mmi\n"
	"  st8 [r32] = r13, 8\n" /* r10 */
	"  nop.m 0\n"
	"  nop.i 0\n"
	"}\n"
	/* Save FP regs f2-f5 */
	"{ .mmf\n"
	"  stf.spill [r32] = f2, 16\n"
	"  stf.spill [r32] = f3, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f4, 16\n"
	"  stf.spill [r32] = f5, 16\n"
	"  nop.f 0\n"
	"}\n"
	/* Save FP regs f16-f31 */
	"{ .mmf\n"
	"  stf.spill [r32] = f16, 16\n"
	"  stf.spill [r32] = f17, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f18, 16\n"
	"  stf.spill [r32] = f19, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f20, 16\n"
	"  stf.spill [r32] = f21, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f22, 16\n"
	"  stf.spill [r32] = f23, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f24, 16\n"
	"  stf.spill [r32] = f25, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f26, 16\n"
	"  stf.spill [r32] = f27, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f28, 16\n"
	"  stf.spill [r32] = f29, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  stf.spill [r32] = f30, 16\n"
	"  stf.spill [r32] = f31, 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mib\n"
	"  mov r8 = r0\n"
	"  nop.i 0\n"
	"  br.ret.sptk.many b0\n"
	"}\n"
	".size __ia64_setjmp, .-__ia64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __ia64_longjmp\n"
	".type __ia64_longjmp, @function\n"
	"__ia64_longjmp:\n"
	"{ .mmi\n"
	"  alloc r2 = ar.pfs, 2, 0, 0, 0\n"
	"  ld8 r12 = [r32], 8\n" /* sp */
	"  ld8 r3 = [r32], 8\n"  /* bsp */
	"}\n"
	"{ .mmi\n"
	"  ld8 r4 = [r32], 8\n"  /* pfs */
	"  ld8 r5 = [r32], 8\n"  /* rnat */
	"  ld8 r6 = [r32], 8\n"  /* unat */
	"}\n"
	"{ .mmi\n"
	"  flushrs\n"
	"  nop.m 0\n"
	"  nop.i 0\n"
	"}\n"
	";;\n"
	"{ .mmi\n"
	"  mov ar.bspstore = r3\n"
	"  mov ar.pfs = r4\n"
	"  mov ar.rnat = r5\n"
	"}\n"
	";;\n"
	"{ .mmi\n"
	"  mov ar.unat = r6\n"
	"  ld8 r6 = [r32], 8\n"  /* gp */
	"  ld8 r7 = [r32], 8\n"  /* b0 */
	"}\n"
	"{ .mmi\n"
	"  mov r1 = r6\n"        /* gp */
	"  mov b0 = r7\n"        /* rp */
	"  ld8 r7 = [r32], 8\n"  /* b1 */
	"}\n"
	"{ .mmi\n"
	"  mov b1 = r7\n"
	"  ld8 r7 = [r32], 8\n"  /* b2 */
	"  ld8 r8 = [r32], 8\n"  /* b3 */
	"}\n"
	"{ .mmi\n"
	"  mov b2 = r7\n"
	"  mov b3 = r8\n"
	"  ld8 r8 = [r32], 8\n"  /* b4 */
	"}\n"
	"{ .mmi\n"
	"  mov b4 = r8\n"
	"  ld8 r8 = [r32], 8\n"  /* b5 */
	"  ld8 r9 = [r32], 8\n"  /* pr */
	"}\n"
	"{ .mmi\n"
	"  mov b5 = r8\n"
	"  mov pr = r9\n"
	"  ld8 r9 = [r32], 8\n"  /* r4 */
	"}\n"
	"{ .mmi\n"
	"  mov r4 = r9\n"
	"  ld8 r9 = [r32], 8\n"  /* r5 */
	"  ld8 r10 = [r32], 8\n" /* r6 */
	"}\n"
	"{ .mmi\n"
	"  mov r5 = r9\n"
	"  mov r6 = r10\n"
	"  ld8 r10 = [r32], 8\n" /* r7 */
	"}\n"
	"{ .mmi\n"
	"  mov r7 = r10\n"
	"  ld8 r10 = [r32], 8\n" /* r8 */
	"  ld8 r11 = [r32], 8\n" /* r9 */
	"}\n"
	"{ .mmi\n"
	"  mov r8 = r10\n"
	"  mov r9 = r11\n"
	"  ld8 r11 = [r32], 8\n" /* r10 */
	"}\n"
	"{ .mmi\n"
	"  mov r10 = r11\n"
	"  nop.m 0\n"
	"  nop.i 0\n"
	"}\n"
	/* Restore FP regs f2-f5 */
	"{ .mmf\n"
	"  ldf.fill f2 = [r32], 16\n"
	"  ldf.fill f3 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f4 = [r32], 16\n"
	"  ldf.fill f5 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	/* Restore FP regs f16-f31 */
	"{ .mmf\n"
	"  ldf.fill f16 = [r32], 16\n"
	"  ldf.fill f17 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f18 = [r32], 16\n"
	"  ldf.fill f19 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f20 = [r32], 16\n"
	"  ldf.fill f21 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f22 = [r32], 16\n"
	"  ldf.fill f23 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f24 = [r32], 16\n"
	"  ldf.fill f25 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f26 = [r32], 16\n"
	"  ldf.fill f27 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f28 = [r32], 16\n"
	"  ldf.fill f29 = [r32], 16\n"
	"  nop.f 0\n"
	"}\n"
	"{ .mmf\n"
	"  ldf.fill f30 = [r32], 16\n"
	"  ldf.fill f31 = [r32], 16\n"
	"  nop.f 0\n"
	";;\n"
	"}\n"
	"{ .mmi\n"
	"  cmp.eq p6, p7 = r0, r33\n"
	"  nop.m 0\n"
	"  nop.i 0\n"
	";;\n"
	"}\n"
	"{ .mib\n"
	"  (p6) mov r8 = 1\n"
	"  (p7) mov r8 = r33\n"
	"  br.ret.sptk.many b0\n"
	";;\n"
	"}\n"
	".size __ia64_longjmp, .-__ia64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
