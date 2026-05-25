/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef microblaze
	#define JACL_ARCH microblaze
	#define JACL_ARCH_MICROBLAZE 1
	#define __jacl_arch_syscall __microblaze_syscall
	#define __jacl_arch_tls_set __microblaze_set_tp_register
	#define __jacl_arch_tls_get __microblaze_get_tp_register
	#define __jacl_arch_clone_thread __microblaze_clone_thread
	#define __jacl_arch_setjmp        __microblaze_setjmp
	#define __jacl_arch_longjmp       __microblaze_longjmp
	#define __jacl_arch_jmpbuf        __microblaze_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __microblaze_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		register long r12 __asm__("r12") = num;
		register long r5 __asm__("r5") = a1;
		register long r6 __asm__("r6") = a2;
		register long r7 __asm__("r7") = a3;
		register long r8 __asm__("r8") = a4;
		register long r9 __asm__("r9") = a5;
		register long r10 __asm__("r10") = a6;
		register long r3 __asm__("r3");

		__asm__ volatile ("brki r14, 0x08"
			: "=r"(r3)
			: "r"(r12), "r"(r5), "r"(r6), "r"(r7), "r"(r8), "r"(r9), "r"(r10)
			: "memory");

		return r3;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"add r19, r0, r0\n"
		"add r5, r1, r0\n"
		"andi r1, r1, -16\n"
		"brlid r15, _start_main\n"
		"nop\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"addik r12, r0, 173\n"
			"brki r14, 8\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __microblaze_set_tp_register(void* addr) {
		__asm__ volatile("add r21, %0, r0" : : "r"(addr) : "memory");
	}

	static inline void* __microblaze_get_tp_register(void) {
		void* result;

		__asm__("add %0, r21, r0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __microblaze_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~3UL) - 4;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"addk r5, r0, %2\n\t"
			"addk r6, r0, %3\n\t"
			"addk r7, r0, r0\n\t"
			"addk r8, r0, r0\n\t"
			"addk r9, r0, r0\n\t"
			"addi r12, r0, 120\n\t"
			"brki r14, 8\n\t"
			"bnei r3, 1f\n\t"

			"addk r19, r0, r0\n\t"
			"addk r5, r0, %5\n\t"
			"brald r15, %4\n\t"
			"addi r12, r0, 1\n\t"
			"brki r14, 8\n\t"

			"1:\n\t"
			: "=r"(ret)
			: "r"((long)120), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r3", "r5", "r6", "r7", "r8", "r9", "r12", "r15", "r19", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __microblaze_jmpbuf[20];

__asm__(
	".text\n"
	".weak __microblaze_setjmp\n"
	".type __microblaze_setjmp, @function\n"
	"__microblaze_setjmp:\n"
	"swi r1, r5, 0\n"
	"swi r15, r5, 4\n"
	"swi r2, r5, 8\n"
	"swi r13, r5, 12\n"
	"swi r18, r5, 16\n"
	"swi r19, r5, 20\n"
	"swi r20, r5, 24\n"
	"swi r21, r5, 28\n"
	"swi r22, r5, 32\n"
	"swi r23, r5, 36\n"
	"swi r24, r5, 40\n"
	"swi r25, r5, 44\n"
	"swi r26, r5, 48\n"
	"swi r27, r5, 52\n"
	"swi r28, r5, 56\n"
	"swi r29, r5, 60\n"
	"swi r30, r5, 64\n"
	"swi r31, r5, 68\n"
	"rtsd r15, 8\n"
	"ori r3, r0, 0\n"
	".size __microblaze_setjmp, .-__microblaze_setjmp\n"
);

__asm__(
	".text\n"
	".weak __microblaze_longjmp\n"
	".type __microblaze_longjmp, @function\n"
	"__microblaze_longjmp:\n"
	"addi r3, r6, 0\n"
	"bnei r3, 1f\n"
	"addi r3, r3, 1\n"
	"1:\n"
	"lwi r1, r5, 0\n"
	"lwi r15, r5, 4\n"
	"lwi r2, r5, 8\n"
	"lwi r13, r5, 12\n"
	"lwi r18, r5, 16\n"
	"lwi r19, r5, 20\n"
	"lwi r20, r5, 24\n"
	"lwi r21, r5, 28\n"
	"lwi r22, r5, 32\n"
	"lwi r23, r5, 36\n"
	"lwi r24, r5, 40\n"
	"lwi r25, r5, 44\n"
	"lwi r26, r5, 48\n"
	"lwi r27, r5, 52\n"
	"lwi r28, r5, 56\n"
	"lwi r29, r5, 60\n"
	"lwi r30, r5, 64\n"
	"lwi r31, r5, 68\n"
	"rtsd r15, 8\n"
	"nop\n"
	".size __microblaze_longjmp, .-__microblaze_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
