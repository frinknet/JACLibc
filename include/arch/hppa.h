/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef hppa
	#define JACL_ARCH hppa
	#define JACL_ARCH_HPPA 1
	#define __jacl_arch_syscall __hppa_syscall
	#define __jacl_arch_tls_set __hppa_set_tp_register
	#define __jacl_arch_tls_get __hppa_get_tp_register
	#define __jacl_arch_clone_thread __hppa_clone_thread
	#define __jacl_arch_setjmp        __hppa_setjmp
	#define __jacl_arch_longjmp       __hppa_longjmp
	#define __jacl_arch_jmpbuf        __hppa_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __hppa_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r20 __asm__("r20") = num;
		register long r26 __asm__("r26") = a1;
		register long r25 __asm__("r25") = a2;
		register long r24 __asm__("r24") = a3;
		register long r23 __asm__("r23") = a4;
		register long r22 __asm__("r22") = a5;
		register long r21 __asm__("r21") = a6;
		register long r28 __asm__("r28");

		__asm__ volatile (
			"ble 0x100(%%sr2, %%r0)\n\t"
			"ldi -1, %%r1"
			: "=r"(r28)
			: "r"(r20), "r"(r26), "r"(r25), "r"(r24), "r"(r23), "r"(r22), "r"(r21)
			: "memory", "r1", "r31");

		return r28;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"ldi 0, %r3\n"
		"copy %sp, %r26\n"
		"ldo 64(%sp), %sp\n"
		"bl _start_main, %r2\n"
		"nop\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"ldi 173, %r20\n"
			"ble 0x100(%%sr2, %%r0)\n"
			"nop\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __hppa_set_tp_register(void* addr) {
		__asm__ volatile("mtctl %0, %%cr27" : : "r"(addr) : "memory");
	}

	static inline void* __hppa_get_tp_register(void) {
		void* result;
		__asm__("mfctl %%cr27, %0" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __hppa_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_bottom = (char *)((uintptr_t)stack & ~63UL) + 64;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r28 __asm__("r28");

		__asm__ volatile(
			"copy %1, %%r26\n\t"
			"copy %2, %%r25\n\t"
			"ldi 0, %%r24\n\t"
			"ldi 0, %%r23\n\t"
			"ldi 0, %%r22\n\t"
			"ldi 120, %%r20\n\t"
			"ble 0x100(%%sr2, %%r0)\n\t"
			"nop\n\t"
			"cmpib,= 0, %%r28, 1f\n\t"
			"nop\n\t"

			"ldi 0, %%r3\n\t"
			"copy %4, %%r26\n\t"
			"ble 0(%%sr0, %3)\n\t"
			"copy %%r31, %%r2\n\t"
			"ldi 1, %%r20\n\t"
			"ble 0x100(%%sr2, %%r0)\n\t"
			"nop\n\t"

			"1:\n\t"
			: "=r"(r28)
			: "r"((long)flags), "r"(stack_bottom), "r"(fn), "r"(arg)
			: "r1", "r2", "r20", "r22", "r23", "r24", "r25", "r26", "memory"
		);

		return (pid_t)r28;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __hppa_jmpbuf[40];

__asm__(
	".text\n"
	".weak __hppa_setjmp\n"
	".type __hppa_setjmp, @function\n"
	"__hppa_setjmp:\n"
	"stw %r3, 0(%r26)\n"
	"stw %r4, 4(%r26)\n"
	"stw %r5, 8(%r26)\n"
	"stw %r6, 12(%r26)\n"
	"stw %r7, 16(%r26)\n"
	"stw %r8, 20(%r26)\n"
	"stw %r9, 24(%r26)\n"
	"stw %r10, 28(%r26)\n"
	"stw %r11, 32(%r26)\n"
	"stw %r12, 36(%r26)\n"
	"stw %r13, 40(%r26)\n"
	"stw %r14, 44(%r26)\n"
	"stw %r15, 48(%r26)\n"
	"stw %r16, 52(%r26)\n"
	"stw %r17, 56(%r26)\n"
	"stw %r18, 60(%r26)\n"
	"stw %r19, 64(%r26)\n"
	"stw %r30, 68(%r26)\n"
	"stw %rp, 72(%r26)\n"
	"ldo 76(%r26), %r1\n"
	"fstds,ma %fr12, 8(%r1)\n"
	"fstds,ma %fr13, 8(%r1)\n"
	"fstds,ma %fr14, 8(%r1)\n"
	"fstds,ma %fr15, 8(%r1)\n"
	"fstds,ma %fr16, 8(%r1)\n"
	"fstds,ma %fr17, 8(%r1)\n"
	"fstds,ma %fr18, 8(%r1)\n"
	"fstds,ma %fr19, 8(%r1)\n"
	"fstds,ma %fr20, 8(%r1)\n"
	"fstds %fr21, 0(%r1)\n"
	"ldi 0, %r28\n"
	"bv,n %r0(%rp)\n"
	".size __hppa_setjmp, .-__hppa_setjmp\n"
);

__asm__(
	".text\n"
	".weak __hppa_longjmp\n"
	".type __hppa_longjmp, @function\n"
	"__hppa_longjmp:\n"
	"copy %r25, %r28\n"
	"ldw 0(%r26), %r3\n"
	"ldw 4(%r26), %r4\n"
	"ldw 8(%r26), %r5\n"
	"ldw 12(%r26), %r6\n"
	"ldw 16(%r26), %r7\n"
	"ldw 20(%r26), %r8\n"
	"ldw 24(%r26), %r9\n"
	"ldw 28(%r26), %r10\n"
	"ldw 32(%r26), %r11\n"
	"ldw 36(%r26), %r12\n"
	"ldw 40(%r26), %r13\n"
	"ldw 44(%r26), %r14\n"
	"ldw 48(%r26), %r15\n"
	"ldw 52(%r26), %r16\n"
	"ldw 56(%r26), %r17\n"
	"ldw 60(%r26), %r18\n"
	"ldw 64(%r26), %r19\n"
	"ldw 68(%r26), %r30\n"
	"ldw 72(%r26), %rp\n"
	"ldo 76(%r26), %r25\n"
	"fldds,ma 8(%r25), %fr12\n"
	"fldds,ma 8(%r25), %fr13\n"
	"fldds,ma 8(%r25), %fr14\n"
	"fldds,ma 8(%r25), %fr15\n"
	"fldds,ma 8(%r25), %fr16\n"
	"fldds,ma 8(%r25), %fr17\n"
	"fldds,ma 8(%r25), %fr18\n"
	"fldds,ma 8(%r25), %fr19\n"
	"fldds,ma 8(%r25), %fr20\n"
	"fldds 0(%r25), %fr21\n"
	"comib,= 0, %r28, 1f\n"
	"nop\n"
	"bv,n %r0(%rp)\n"
	"1:\n"
	"ldi 1, %r28\n"
	"bv,n %r0(%rp)\n"
	".size __hppa_longjmp, .-__hppa_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
