/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef x64
	#define JACL_ARCH x64
	#define JACL_ARCH_X64 1
	#define __jacl_arch_syscall       __x64_syscall
	#define __jacl_arch_tls_set       __x64_set_fs_register
	#define __jacl_arch_tls_get       __x64_get_fs_register
	#define __jacl_arch_clone_thread  __x64_clone_thread
	#define __jacl_arch_setjmp        __x64_setjmp
	#define __jacl_arch_longjmp       __x64_longjmp
	#define __jacl_arch_jmpbuf        __x64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL

static inline long __x64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	register long rax __asm__("rax") = num;
	register long rdi __asm__("rdi") = a1;
	register long rsi __asm__("rsi") = a2;
	register long rdx __asm__("rdx") = a3;
	register long r10 __asm__("r10") = a4;
	register long r8 __asm__("r8")   = a5;
	register long r9 __asm__("r9")   = a6;

	__asm__ volatile ("syscall"
		: "=a"(result)
		: "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx), "r"(r10), "r"(r8), "r"(r9)
		: "memory", "rcx", "r11");

	return result;
}

#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START

__asm__(
	".text\n"
	".globl _start\n"
	"_start:\n"
	"xor %rbp,%rbp\n"
	"mov %rsp,%rdi\n"
	"andq $-16,%rsp\n"
	"call _start_main\n"
);

#if JACL_OS_LINUX
	__asm__(
		".text\n"
		".globl __restore_rt\n"
		".type __restore_rt,@function\n"
		"__restore_rt:\n"
		"mov $15, %rax\n"
		"syscall\n"
	);
#endif

#undef __ARCH_START
#endif

#ifdef __ARCH_TLS

// Only CPU register manipulation
static inline void __x64_set_fs_register(void* addr) {
	__asm__ volatile("mov %0, %%fs:0" : : "r"(addr) : "memory");
}

static inline void* __x64_get_fs_register(void) {
	void* result;

	__asm__("mov %%fs:0, %0" : "=r"(result));

	return result;
}

#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX

static inline pid_t __x64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
	char *stack_top = (char *)stack + stack_size;
	stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

	int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
	long ret;

	__asm__ volatile(
		"mov $56, %%rax\n\t"         /* SYS_clone */
		"mov %2, %%rdi\n\t"          /* flags */
		"mov %3, %%rsi\n\t"          /* child_stack */
		"xor %%rdx, %%rdx\n\t"       /* parent_tid = NULL */
		"xor %%r10, %%r10\n\t"       /* child_tid = NULL */
		"syscall\n\t"
		"test %%rax, %%rax\n\t"      /* Check return value */
		"jnz 1f\n\t"                 /* Parent if non-zero */

		/* Child path (rax == 0) */
		"xor %%rbp, %%rbp\n\t"
		"mov %5, %%rdi\n\t"          /* arg */
		"call *%4\n\t"               /* fn(arg) */
		"mov %%rax, %%rdi\n\t"
		"mov $60, %%rax\n\t"         /* SYS_exit */
		"syscall\n\t"

		"1:\n\t"                     /* Parent continues */
		: "=a"(ret)
		: "a"((long)56), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
		: "rcx", "r11", "rdx", "rsi", "rdi", "r8", "r9", "r10", "memory"
	);

	return ret;
}

#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __x64_jmpbuf[8];

__asm__(
	".text\n"
	".weak __x64_setjmp\n"
	".type __x64_setjmp, @function\n"
	"__x64_setjmp:\n"
	"mov %rbx, (%rdi)\n"
	"mov %rbp, 8(%rdi)\n"
	"mov %r12, 16(%rdi)\n"
	"mov %r13, 24(%rdi)\n"
	"mov %r14, 32(%rdi)\n"
	"mov %r15, 40(%rdi)\n"
	"lea 8(%rsp), %rdx\n"
	"mov %rdx, 48(%rdi)\n"
	"mov (%rsp), %rdx\n"
	"mov %rdx, 56(%rdi)\n"
	"xor %eax, %eax\n"
	"ret\n"
	".size __x64_setjmp, .-__x64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __x64_longjmp\n"
	".type __x64_longjmp, @function\n"
	"__x64_longjmp:\n"
	"xor %eax,%eax\n"
	"cmp $1,%esi\n"
	"adc %esi,%eax\n"
	"mov (%rdi),%rbx\n"
	"mov 8(%rdi),%rbp\n"
	"mov 16(%rdi),%r12\n"
	"mov 24(%rdi),%r13\n"
	"mov 32(%rdi),%r14\n"
	"mov 40(%rdi),%r15\n"
	"mov 48(%rdi),%rsp\n"
	"jmp *56(%rdi)\n"
	".size __x64_longjmp, .-__x64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
}
#endif
