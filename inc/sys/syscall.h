/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <unistd.h>

/* ================================================================ */
/* Architecture-specific syscall numbers                           */
/* ================================================================ */

#if defined(__x86_64__) || defined(__amd64__)
/* x86_64 Linux syscall numbers */
#define SYS_read      0
#define SYS_write     1
#define SYS_open      2
#define SYS_close     3
#define SYS_stat      4
#define SYS_fstat     5
#define SYS_lstat     6
#define SYS_poll      7
#define SYS_lseek     8
#define SYS_mmap      9
#define SYS_mprotect  10
#define SYS_munmap    11
#define SYS_brk       12
#define SYS_rt_sigaction  13
#define SYS_rt_sigprocmask 14
#define SYS_ioctl     16
#define SYS_pread64   17
#define SYS_pwrite64  18
#define SYS_access    21
#define SYS_pipe      22
#define SYS_select    23
#define SYS_sched_yield 24
#define SYS_msync     26
#define SYS_mincore   27
#define SYS_madvise   28
#define SYS_dup       32
#define SYS_dup2      33
#define SYS_pause     34
#define SYS_nanosleep 35
#define SYS_getitimer 36
#define SYS_alarm     37
#define SYS_setitimer 38
#define SYS_getpid    39
#define SYS_sendfile  40
#define SYS_socket    41
#define SYS_connect   42
#define SYS_accept    43
#define SYS_sendto    44
#define SYS_recvfrom  45
#define SYS_sendmsg   46
#define SYS_recvmsg   47
#define SYS_shutdown  48
#define SYS_bind      49
#define SYS_listen    50
#define SYS_getsockname 51
#define SYS_getpeername 52
#define SYS_socketpair  53
#define SYS_setsockopt  54
#define SYS_getsockopt  55
#define SYS_clone     56
#define SYS_fork      57
#define SYS_vfork     58
#define SYS_execve    59
#define SYS_exit      60
#define SYS_wait4     61
#define SYS_kill      62
#define SYS_uname     63
#define SYS_fcntl     72
#define SYS_flock     73
#define SYS_fsync     74
#define SYS_fdatasync 75
#define SYS_truncate  76
#define SYS_ftruncate 77
#define SYS_getdents  78
#define SYS_getcwd    79
#define SYS_chdir     80
#define SYS_fchdir    81
#define SYS_rename    82
#define SYS_mkdir     83
#define SYS_rmdir     84
#define SYS_creat     85
#define SYS_link      86
#define SYS_unlink    87
#define SYS_symlink   88
#define SYS_readlink  89
#define SYS_chmod     90
#define SYS_fchmod    91
#define SYS_chown     92
#define SYS_fchown    93
#define SYS_lchown    94
#define SYS_umask     95
#define SYS_gettimeofday 96
#define SYS_getrlimit    97
#define SYS_getrusage    98
#define SYS_sysinfo      99
#define SYS_times        100
#define SYS_ptrace       101
#define SYS_getuid       102
#define SYS_syslog       103
#define SYS_getgid       104
#define SYS_setuid       105
#define SYS_setgid       106
#define SYS_geteuid      107
#define SYS_getegid      108
#define SYS_setpgid      109
#define SYS_getppid      110
#define SYS_getpgrp      111
#define SYS_setsid       112
#define SYS_setreuid     113
#define SYS_setregid     114
#define SYS_getgroups    115
#define SYS_setgroups    116
#define SYS_setresuid    117
#define SYS_getresuid    118
#define SYS_setresgid    119
#define SYS_getresgid    120
#define SYS_getpgid      121
#define SYS_setfsuid     122
#define SYS_setfsgid     123
#define SYS_getsid       124
#define SYS_capget       125
#define SYS_capset       126
#define SYS_sigpending   127
#define SYS_sigtimedwait 128
#define SYS_sigqueueinfo 129
#define SYS_sigreturn    130
#define SYS_utime        132
#define SYS_mknod        133
#define SYS_personality  135
#define SYS_ustat        136
#define SYS_statfs       137
#define SYS_fstatfs      138
#define SYS_sysfs        139
#define SYS_getpriority  140
#define SYS_setpriority  141
#define SYS_sched_setparam    142
#define SYS_sched_getparam    143
#define SYS_sched_setscheduler 144
#define SYS_sched_getscheduler 145
#define SYS_sched_get_priority_max 146
#define SYS_sched_get_priority_min 147
#define SYS_sched_rr_get_interval  148
#define SYS_mlock        149
#define SYS_munlock      150
#define SYS_mlockall     151
#define SYS_munlockall   152
#define SYS_vhangup      153
#define SYS_pivot_root   155
#define SYS_sysctl       156
#define SYS_prctl        157
#define SYS_arch_prctl   158
#define SYS_adjtimex     159
#define SYS_setrlimit    160
#define SYS_chroot       161
#define SYS_sync         162
#define SYS_acct         163
#define SYS_settimeofday 164
#define SYS_mount        165
#define SYS_umount2      166
#define SYS_swapon       167
#define SYS_swapoff      168
#define SYS_reboot       169
#define SYS_sethostname  170
#define SYS_setdomainname 171
#define SYS_iopl         172
#define SYS_ioperm       173
#define SYS_init_module  175
#define SYS_delete_module 176
#define SYS_quotactl     179
#define SYS_gettid       186
#define SYS_readahead    187
#define SYS_setxattr     188
#define SYS_lsetxattr    189
#define SYS_fsetxattr    190
#define SYS_getxattr     191
#define SYS_lgetxattr    192
#define SYS_fgetxattr    193
#define SYS_listxattr    194
#define SYS_llistxattr   195
#define SYS_flistxattr   196
#define SYS_removexattr  197
#define SYS_lremovexattr 198
#define SYS_fremovexattr 199
#define SYS_tkill        200
#define SYS_time         201
#define SYS_futex        202
#define SYS_sched_setaffinity 203
#define SYS_sched_getaffinity 204
#define SYS_io_setup     206
#define SYS_io_destroy   207
#define SYS_io_getevents 208
#define SYS_io_submit    209
#define SYS_io_cancel    210
#define SYS_lookup_dcookie 212
#define SYS_epoll_create   213
#define SYS_remap_file_pages 216
#define SYS_getdents64   217
#define SYS_set_tid_address 218
#define SYS_restart_syscall 219
#define SYS_semtimedop   220
#define SYS_fadvise64    221
#define SYS_timer_create 222
#define SYS_timer_settime 223
#define SYS_timer_gettime 224
#define SYS_timer_getoverrun 225
#define SYS_timer_delete 226
#define SYS_clock_settime 227
#define SYS_clock_gettime 228
#define SYS_clock_getres  229
#define SYS_clock_nanosleep 230
#define SYS_exit_group   231
#define SYS_epoll_wait   232
#define SYS_epoll_ctl    233
#define SYS_tgkill       234
#define SYS_utimes       235
#define SYS_mbind        237
#define SYS_set_mempolicy 238
#define SYS_get_mempolicy 239
#define SYS_mq_open      240
#define SYS_mq_unlink    241
#define SYS_mq_timedsend 242
#define SYS_mq_timedreceive 243
#define SYS_mq_notify    244
#define SYS_mq_getsetattr 245
#define SYS_waitid       247

#elif defined(__aarch64__)
/* ARM64 syscall numbers */
#define SYS_io_setup 0
#define SYS_io_destroy 1
#define SYS_io_submit 2
#define SYS_io_cancel 3
#define SYS_io_getevents 4
#define SYS_setxattr 5
#define SYS_lsetxattr 6
#define SYS_fsetxattr 7
#define SYS_getxattr 8
#define SYS_lgetxattr 9
#define SYS_fgetxattr 10
#define SYS_listxattr 11
#define SYS_llistxattr 12
#define SYS_flistxattr 13
#define SYS_removexattr 14
#define SYS_lremovexattr 15
#define SYS_fremovexattr 16
#define SYS_getcwd 17
#define SYS_lookup_dcookie 18
#define SYS_eventfd2 19
#define SYS_epoll_create1 20
#define SYS_epoll_ctl 21
#define SYS_epoll_pwait 22
#define SYS_dup 23
#define SYS_dup3 24
#define SYS_fcntl 25
#define SYS_inotify_init1 26
#define SYS_inotify_add_watch 27
#define SYS_inotify_rm_watch 28
#define SYS_ioctl 29
#define SYS_ioprio_set 30
#define SYS_ioprio_get 31
#define SYS_flock 32
#define SYS_mknodat 33
#define SYS_mkdirat 34
#define SYS_unlinkat 35
#define SYS_symlinkat 36
#define SYS_linkat 37
#define SYS_renameat 38
#define SYS_umount2 39
#define SYS_mount 40
#define SYS_pivot_root 41
#define SYS_nfsservctl 42
#define SYS_statfs 43
#define SYS_fstatfs 44
#define SYS_truncate 45
#define SYS_ftruncate 46
#define SYS_fallocate 47
#define SYS_faccessat 48
#define SYS_chdir 49
#define SYS_fchdir 50
#define SYS_chroot 51
#define SYS_fchmod 52
#define SYS_fchmodat 53
#define SYS_fchownat 54
#define SYS_fchown 55
#define SYS_openat 56
#define SYS_close 57
#define SYS_vhangup 58
#define SYS_pipe2 59
#define SYS_quotactl 60
#define SYS_getdents64 61
#define SYS_lseek 62
#define SYS_read 63
#define SYS_write 64
#define SYS_readv 65
#define SYS_writev 66
#define SYS_pread64 67
#define SYS_pwrite64 68
#define SYS_preadv 69
#define SYS_pwritev 70
#define SYS_sendfile 71
#define SYS_pselect6 72
#define SYS_ppoll 73
#define SYS_signalfd4 74
#define SYS_vmsplice 75
#define SYS_splice 76
#define SYS_tee 77
#define SYS_readlinkat 78
#define SYS_fstatat 79
#define SYS_fstat 80
#define SYS_sync 81
#define SYS_fsync 82
#define SYS_fdatasync 83
#define SYS_sync_file_range 84
#define SYS_timerfd_create 85
#define SYS_timerfd_settime 86
#define SYS_timerfd_gettime 87
#define SYS_utimensat 88
#define SYS_acct 89
#define SYS_capget 90
#define SYS_capset 91
#define SYS_personality 92
#define SYS_exit 93
#define SYS_exit_group 94
#define SYS_waitid 95
#define SYS_set_tid_address 96
#define SYS_unshare 97
#define SYS_futex 98
#define SYS_set_robust_list 99
#define SYS_get_robust_list 100
#define SYS_nanosleep 101
#define SYS_getitimer 102
#define SYS_setitimer 103
#define SYS_kexec_load 104
#define SYS_init_module 105
#define SYS_delete_module 106
#define SYS_timer_create 107
#define SYS_timer_gettime 108
#define SYS_timer_getoverrun 109
#define SYS_timer_settime 110
#define SYS_timer_delete 111
#define SYS_clock_settime 112
#define SYS_clock_gettime 113
#define SYS_clock_getres 114
#define SYS_clock_nanosleep 115
#define SYS_syslog 116
#define SYS_ptrace 117
#define SYS_sched_setparam 118
#define SYS_sched_setscheduler 119
#define SYS_sched_getscheduler 120
#define SYS_sched_getparam 121
#define SYS_sched_setaffinity 122
#define SYS_sched_getaffinity 123
#define SYS_sched_yield 124
#define SYS_sched_get_priority_max 125
#define SYS_sched_get_priority_min 126
#define SYS_sched_rr_get_interval 127
#define SYS_restart_syscall 128
#define SYS_kill 129
#define SYS_tkill 130
#define SYS_tgkill 131
#define SYS_sigaltstack 132
#define SYS_rt_sigsuspend 133
#define SYS_rt_sigaction 134
#define SYS_rt_sigprocmask 135
#define SYS_rt_sigpending 136
#define SYS_rt_sigtimedwait 137
#define SYS_rt_sigqueueinfo 138
#define SYS_rt_sigreturn 139
#define SYS_setpriority 140
#define SYS_getpriority 141
#define SYS_reboot 142
#define SYS_setregid 143
#define SYS_setgid 144
#define SYS_setreuid 145
#define SYS_setuid 146
#define SYS_setresuid 147
#define SYS_getresuid 148
#define SYS_setresgid 149
#define SYS_getresgid 150
#define SYS_setfsuid 151
#define SYS_setfsgid 152
#define SYS_times 153
#define SYS_setpgid 154
#define SYS_getpgid 155
#define SYS_getsid 156
#define SYS_setsid 157
#define SYS_getgroups 158
#define SYS_setgroups 159
#define SYS_uname 160
#define SYS_sethostname 161
#define SYS_setdomainname 162
#define SYS_getrlimit 163
#define SYS_setrlimit 164
#define SYS_getrusage 165
#define SYS_umask 166
#define SYS_prctl 167
#define SYS_getcpu 168
#define SYS_gettimeofday 169
#define SYS_settimeofday 170
#define SYS_adjtimex 171
#define SYS_getpid 172
#define SYS_getppid 173
#define SYS_getuid 174
#define SYS_geteuid 175
#define SYS_getgid 176
#define SYS_getegid 177
#define SYS_gettid 178
#define SYS_sysinfo 179
#define SYS_mq_open 180
#define SYS_mq_unlink 181
#define SYS_mq_timedsend 182
#define SYS_mq_timedreceive 183
#define SYS_mq_notify 184
#define SYS_mq_getsetattr 185
#define SYS_msgget 186
#define SYS_msgctl 187
#define SYS_msgrcv 188
#define SYS_msgsnd 189
#define SYS_semget 190
#define SYS_semctl 191
#define SYS_semtimedop 192
#define SYS_semop 193
#define SYS_shmget 194
#define SYS_shmctl 195
#define SYS_shmat 196
#define SYS_shmdt 197
#define SYS_socket 198
#define SYS_socketpair 199
#define SYS_bind 200
#define SYS_listen 201
#define SYS_accept 202
#define SYS_connect 203
#define SYS_getsockname 204
#define SYS_getpeername 205
#define SYS_sendto 206
#define SYS_recvfrom 207
#define SYS_setsockopt 208
#define SYS_getsockopt 209
#define SYS_shutdown 210
#define SYS_sendmsg 211
#define SYS_recvmsg 212
#define SYS_readahead 213
#define SYS_brk 214
#define SYS_munmap 215
#define SYS_mremap 216
#define SYS_add_key 217
#define SYS_request_key 218
#define SYS_keyctl 219
#define SYS_clone 220
#define SYS_execve 221
#define SYS_mmap 222
#define SYS_fadvise64 223
#define SYS_swapon 224
#define SYS_swapoff 225
#define SYS_mprotect 226
#define SYS_msync 227
#define SYS_mlock 228
#define SYS_munlock 229
#define SYS_mlockall 230
#define SYS_munlockall 231
#define SYS_mincore 232
#define SYS_madvise 233

#else
/* Generic fallback syscall numbers - basic set */
#define SYS_read      0
#define SYS_write     1
#define SYS_open      2
#define SYS_close     3
#define SYS_stat      4
#define SYS_fstat     5
#define SYS_lstat     6
#define SYS_lseek     8
#define SYS_mmap      9
#define SYS_mprotect  10
#define SYS_munmap    11
#define SYS_brk       12
#define SYS_nanosleep 35
#define SYS_getpid    39
#define SYS_clone     56
#define SYS_fork      57
#define SYS_execve    59
#define SYS_exit      60
#define SYS_kill      62
#define SYS_uname     63
#define SYS_fcntl     72
#define SYS_getcwd    79
#define SYS_chdir     80
#define SYS_rename    82
#define SYS_mkdir     83
#define SYS_rmdir     84
#define SYS_unlink    87
#define SYS_chmod     90
#define SYS_chown     92
#define SYS_getuid    102
#define SYS_getgid    104
#define SYS_setuid    105
#define SYS_setgid    106
#define SYS_gettid    186
#define SYS_futex     202
#endif

/* ================================================================ */
/* syscall() function implementation                               */
/* ================================================================ */

#if defined(__linux__)
/* Linux syscall implementation */

#if defined(__x86_64__)
static inline long syscall_impl(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	register long rax __asm__("rax") = number;
	register long rdi __asm__("rdi") = a1;
	register long rsi __asm__("rsi") = a2;
	register long rdx __asm__("rdx") = a3;
	register long r10 __asm__("r10") = a4;
	register long r8 __asm__("r8") = a5;
	register long r9 __asm__("r9") = a6;
	
	__asm__ volatile ("syscall"
		: "=a"(result)
		: "r"(rax), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9)
		: "memory", "rcx", "r11");
	
	return result;
}

#elif defined(__aarch64__)
static inline long syscall_impl(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	register long x8 __asm__("x8") = number;
	register long x0 __asm__("x0") = a1;
	register long x1 __asm__("x1") = a2;
	register long x2 __asm__("x2") = a3;
	register long x3 __asm__("x3") = a4;
	register long x4 __asm__("x4") = a5;
	register long x5 __asm__("x5") = a6;
	
	__asm__ volatile ("svc #0"
		: "=r"(x0)
		: "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
		: "memory");
	
	result = x0;
	return result;
}

#elif defined(__i386__)
static inline long syscall_impl(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	__asm__ volatile ("int $0x80"
		: "=a"(result)
		: "a"(number), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5)
		: "memory");
	(void)a6; /* i386 only supports 5 args */
	return result;
}

#else
/* Generic fallback - try to use libc syscall if available */
extern long syscall(long number, ...);
static inline long syscall_impl(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
	return syscall(number, a1, a2, a3, a4, a5, a6);
}
#endif

/* Variadic syscall wrapper */
static inline long syscall(long number, ...) {
	va_list args;
	long a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0;
	
	va_start(args, number);
	a1 = va_arg(args, long);
	a2 = va_arg(args, long);
	a3 = va_arg(args, long);
	a4 = va_arg(args, long);
	a5 = va_arg(args, long);
	a6 = va_arg(args, long);
	va_end(args);
	
	return syscall_impl(number, a1, a2, a3, a4, a5, a6);
}

#else
/* Non-Linux platforms */
static inline long syscall(long number, ...) {
	(void)number;
	return -1; /* Not supported */
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* SYS_SYSCALL_H */

