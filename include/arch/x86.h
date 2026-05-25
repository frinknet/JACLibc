/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef x86
	#define JACL_ARCH x86
	#define JACL_ARCH_X86 1
	#define __jacl_arch_syscall __x86_syscall
	#define __jacl_arch_tls_set __x86_set_gs_register
	#define __jacl_arch_tls_get __x86_get_gs_register
	#define __jacl_arch_clone_thread __x86_clone_thread
	#define __jacl_arch_setjmp        __x86_setjmp
	#define __jacl_arch_longjmp       __x86_longjmp
	#define __jacl_arch_jmpbuf        __x86_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __x86_syscall(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		__asm__ volatile (
			"pushl %%ebp\n\t"
			"movl %7, %%ebp\n\t"
			"int $0x80\n\t"
			"popl %%ebp"
			: "=a"(result)
			: "a"(number), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5), "r"(a6)
			: "memory"
		);
		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".text\n"
		".globl _start\n"
		"_start:\n"
		"xor %ebp,%ebp\n"
		"mov %esp,%eax\n"
		"and $-16,%esp\n"
		"push %eax\n"
		"push %eax\n"
		"call _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"mov $173, %eax\n"
			"int $0x80\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	struct __x86_user_desc {
		unsigned int  entry_number;
		unsigned int  base_addr;
		unsigned int  limit;
		unsigned int  seg_32bit:1;
		unsigned int  contents:2;
		unsigned int  read_exec_only:1;
		unsigned int  limit_in_pages:1;
		unsigned int  seg_not_present:1;
		unsigned int  useable:1;
	};

	static inline void __x86_set_gs_register(void* addr) {
		struct __x86_user_desc desc = {
			.entry_number    = -1,
			.base_addr       = (unsigned int)addr,
			.limit           = 0xfffff,
			.seg_32bit       = 1,
			.contents        = 0,
			.read_exec_only  = 0,
			.limit_in_pages  = 1,
			.seg_not_present = 0,
			.useable         = 1
		};
		long res = __x86_syscall(243, (long)&desc, 0, 0, 0, 0, 0);
		if (res >= 0) {
			unsigned int selector = (desc.entry_number << 3) | 3;
			__asm__ volatile("movw %w0, %%gs" : : "r"(selector));
		}
	}

	static inline void* __x86_get_gs_register(void) {
		void* result;
		__asm__("movl %%gs:0, %0" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __x86_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		unsigned long *stack_ptr = (unsigned long *)((char *)stack + stack_size);

		/* Pre-pack parameters onto the child stack */
		*(--stack_ptr) = (unsigned long)arg;
		*(--stack_ptr) = (unsigned long)fn;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"movl $120, %%eax\n\t" /* SYS_clone */
			"movl %1, %%ebx\n\t"   /* flags */
			"movl %2, %%ecx\n\t"   /* stack_top */
			"xor %%edx, %%edx\n\t"
			"xor %%esi, %%esi\n\t"
			"xor %%edi, %%edi\n\t"
			"int $0x80\n\t"
			"test %%eax, %%eax\n\t"
			"jnz 1f\n\t"

			/* Child Thread Context */
			"xor %%ebp, %%ebp\n\t"
			"popl %%eax\n\t"       /* fn */
			"popl %%ecx\n\t"       /* arg */
			"pushl %%ecx\n\t"
			"call *%%eax\n\t"
			"movl $1, %%eax\n\t"   /* SYS_exit */
			"int $0x80\n\t"

			"1:\n\t"
			: "=a"(ret)
			: "r"((long)flags), "r"(stack_ptr)
			: "ebx", "ecx", "edx", "esi", "edi", "memory"
		);

		return (pid_t)ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __x86_jmpbuf[6];

__asm__(
	".text\n"
	".weak __x86_setjmp\n"
	".type __x86_setjmp, @function\n"
	"__x86_setjmp:\n"
	"mov 4(%esp), %eax\n"
	"mov %ebx, (%eax)\n"
	"mov %esi, 4(%eax)\n"
	"mov %edi, 8(%eax)\n"
	"mov %ebp, 12(%eax)\n"
	"lea 4(%esp), %ecx\n"
	"mov %ecx, 16(%eax)\n"
	"mov (%esp), %ecx\n"
	"mov %ecx, 20(%eax)\n"
	"xor %eax, %eax\n"
	"ret\n"
	".size __x86_setjmp, .-__x86_setjmp\n"
);

__asm__(
	".text\n"
	".weak __x86_longjmp\n"
	".type __x86_longjmp, @function\n"
	"__x86_longjmp:\n"
	"mov 4(%esp), %edx\n"
	"mov 8(%esp), %eax\n"
	"cmp $1, %eax\n"
	"adc $0, %al\n"
	"mov (%edx), %ebx\n"
	"mov 4(%edx), %esi\n"
	"mov 8(%edx), %edi\n"
	"mov 12(%edx), %ebp\n"
	"mov 16(%edx), %esp\n"
	"jmp *20(%edx)\n"
	".size __x86_longjmp, .-__x86_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
