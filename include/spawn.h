/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SPAWN_H
#define _SPAWN_H

#pragma once

#include <config.h>
#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Types                                                                        */
/* ============================================================================ */

typedef struct {
	short flags;
	sigset_t sigmask;
	sigset_t sigdefault;
	struct sched_param spawn_schedparam;
	int spawn_schedpolicy;
	pid_t spawn_pgroup;
} posix_spawnattr_t;

typedef struct {
	int allocated;
	int used;
	struct {
		int action;
		union {
			int fd;
			int newfd;
			struct {
				char *path;
				int oflag;
				mode_t mode;
			} open;
		} data;
	} *actions;
} posix_spawn_file_actions_t;

/* Flags */
#define POSIX_SPAWN_RESETIDS       0x01
#define POSIX_SPAWN_SETPGROUP      0x02
#define POSIX_SPAWN_SETSIGDEF      0x04
#define POSIX_SPAWN_SETSIGMASK     0x08
#define POSIX_SPAWN_SETSCHEDPARAM  0x10
#define POSIX_SPAWN_SETSCHEDULER   0x20
#define POSIX_SPAWN_USEVFORK       0x40
#define POSIX_SPAWN_SETSID         0x80

/* File Actions */
#define POSIX_SPAWN_OPEN           0
#define POSIX_SPAWN_CLOSE          1
#define POSIX_SPAWN_DUP2           2

/* ============================================================================ */
/* Attribute Functions                                                          */
/* ============================================================================ */

static inline int posix_spawnattr_init(posix_spawnattr_t *attr) {
	if (!attr) return EINVAL;
	attr->flags = 0;
	sigemptyset(&attr->sigmask);
	sigemptyset(&attr->sigdefault);
	attr->spawn_pgroup = 0;
	attr->spawn_schedpolicy = 0;
	memset(&attr->spawn_schedparam, 0, sizeof(attr->spawn_schedparam));
	return 0;
}

static inline int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
	if (!attr) return EINVAL;
	return 0;
}

static inline int posix_spawnattr_getflags(const posix_spawnattr_t *restrict attr, short *restrict flags) {
	if (!attr || !flags) return EINVAL;
	*flags = attr->flags;
	return 0;
}

static inline int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
	if (!attr) return EINVAL;
	attr->flags = flags;
	return 0;
}

static inline int posix_spawnattr_getpgroup(const posix_spawnattr_t *restrict attr, pid_t *restrict pgroup) {
	if (!attr || !pgroup) return EINVAL;
	*pgroup = attr->spawn_pgroup;
	return 0;
}

static inline int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup) {
	if (!attr) return EINVAL;
	attr->spawn_pgroup = pgroup;
	return 0;
}

static inline int posix_spawnattr_getsigmask(const posix_spawnattr_t *restrict attr, sigset_t *restrict sigmask) {
	if (!attr || !sigmask) return EINVAL;
	*sigmask = attr->sigmask;
	return 0;
}

static inline int posix_spawnattr_setsigmask(posix_spawnattr_t *restrict attr, const sigset_t *restrict sigmask) {
	if (!attr || !sigmask) return EINVAL;
	attr->sigmask = *sigmask;
	return 0;
}

static inline int posix_spawnattr_getsigdefault(const posix_spawnattr_t *restrict attr, sigset_t *restrict sigdefault) {
	if (!attr || !sigdefault) return EINVAL;
	*sigdefault = attr->sigdefault;
	return 0;
}

static inline int posix_spawnattr_setsigdefault(posix_spawnattr_t *restrict attr, const sigset_t *restrict sigdefault) {
	if (!attr || !sigdefault) return EINVAL;
	attr->sigdefault = *sigdefault;
	return 0;
}

static inline int posix_spawnattr_getschedparam(const posix_spawnattr_t *restrict attr, struct sched_param *restrict param) {
	if (!attr || !param) return EINVAL;
	*param = attr->spawn_schedparam;
	return 0;
}
static inline int posix_spawnattr_setschedparam(posix_spawnattr_t *restrict attr, const struct sched_param *restrict param) {
	if (!attr || !param) return EINVAL;
	attr->spawn_schedparam = *param;
	return 0;
}
static inline int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *restrict attr, int *restrict policy) {
	if (!attr || !policy) return EINVAL;
	*policy = attr->spawn_schedpolicy;
	return 0;
}
static inline int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int policy) {
	if (!attr) return EINVAL;
	attr->spawn_schedpolicy = policy;
	return 0;
}

/* ============================================================================ */
/* File Actions Functions                                                       */
/* ============================================================================ */

static inline int __jacl_spawn_action_add(posix_spawn_file_actions_t *file_actions, int action) {
	if (file_actions->used >= file_actions->allocated) {
		int new_alloc = file_actions->allocated ? file_actions->allocated * 2 : 4;
		void *new_actions = realloc(file_actions->actions, new_alloc * sizeof(*file_actions->actions));
		if (!new_actions) return ENOMEM;
		file_actions->actions = new_actions;
		file_actions->allocated = new_alloc;
	}
	file_actions->actions[file_actions->used].action = action;
	file_actions->used++;
	return 0;
}

static inline int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
	if (!file_actions) return EINVAL;
	file_actions->allocated = 0;
	file_actions->used = 0;
	file_actions->actions = NULL;
	return 0;
}

static inline int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
	if (!file_actions) return EINVAL;
	if (file_actions->actions) {
		for (int i = 0; i < file_actions->used; i++) {
			if (file_actions->actions[i].action == POSIX_SPAWN_OPEN) {
				free(file_actions->actions[i].data.open.path);
			}
		}
		free(file_actions->actions);
	}
	file_actions->actions = NULL;
	file_actions->used = 0;
	file_actions->allocated = 0;
	return 0;
}

static inline int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *restrict file_actions, int fd, const char *restrict path, int oflag, mode_t mode) {
	if (!file_actions || !path) return EINVAL;
	int r = __jacl_spawn_action_add(file_actions, POSIX_SPAWN_OPEN);
	if (r) return r;
	
	int idx = file_actions->used - 1;
	file_actions->actions[idx].data.fd = fd;
	file_actions->actions[idx].data.open.oflag = oflag;
	file_actions->actions[idx].data.open.mode = mode;
	file_actions->actions[idx].data.open.path = strdup(path);
	if (!file_actions->actions[idx].data.open.path) {
		file_actions->used--; 
		return ENOMEM;
	}
	return 0;
}

static inline int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions, int fd) {
	if (!file_actions) return EINVAL;
	int r = __jacl_spawn_action_add(file_actions, POSIX_SPAWN_CLOSE);
	if (r) return r;
	file_actions->actions[file_actions->used - 1].data.fd = fd;
	return 0;
}

static inline int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions, int fd, int newfd) {
	if (!file_actions) return EINVAL;
	int r = __jacl_spawn_action_add(file_actions, POSIX_SPAWN_DUP2);
	if (r) return r;
	file_actions->actions[file_actions->used - 1].data.fd = fd;
	file_actions->actions[file_actions->used - 1].data.newfd = newfd;
	return 0;
}

/* ============================================================================ */
/* Spawn Functions                                                              */
/* ============================================================================ */

#if defined(JACL_OS_WINDOWS) || defined(JACL_ARCH_WASM)

static inline int posix_spawn(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
	(void)pid; (void)path; (void)file_actions; (void)attrp; (void)argv; (void)envp;
	return ENOSYS;
}

static inline int posix_spawnp(pid_t *pid, const char *file, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
	(void)pid; (void)file; (void)file_actions; (void)attrp; (void)argv; (void)envp;
	return ENOSYS;
}

#else /* POSIX */

static inline int posix_spawn(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
	if (!path || !argv) return EINVAL;

	pid_t p;

	/* Use vfork for performance if available and not Darwin (where vfork==fork) */
	#if defined(SYS_vfork) && !defined(JACL_OS_DARWIN)
		p = vfork();
	#else
		p = fork();
	#endif

	if (p < 0) return errno;

	if (p == 0) {
		/* Child Process - CRITICAL: No malloc/free/printf */
		
		/* 1. Handle Attributes */
		if (attrp) {
			if (attrp->flags & POSIX_SPAWN_SETSIGMASK) {
				sigprocmask(SIG_SETMASK, &attrp->sigmask, NULL);
			}
			if (attrp->flags & POSIX_SPAWN_SETSIGDEF) {
				for (int sig = 1; sig < NSIG; sig++) {
					if (sigismember(&attrp->sigdefault, sig)) {
						signal(sig, SIG_DFL);
					}
				}
			}
			if (attrp->flags & POSIX_SPAWN_SETPGROUP) {
				setpgid(0, attrp->spawn_pgroup);
			}
			if (attrp->flags & POSIX_SPAWN_SETSID) {
				setsid();
			}
		}

		/* 2. Handle File Actions */
		if (file_actions) {
			for (int i = 0; i < file_actions->used; i++) {
				switch (file_actions->actions[i].action) {
					case POSIX_SPAWN_OPEN: {
						int fd = open(file_actions->actions[i].data.open.path, 
						              file_actions->actions[i].data.open.oflag, 
						              file_actions->actions[i].data.open.mode);
						if (fd >= 0) {
							if (fd != file_actions->actions[i].data.fd) {
								dup2(fd, file_actions->actions[i].data.fd);
								close(fd);
							}
						}
						break;
					}
					case POSIX_SPAWN_CLOSE:
						close(file_actions->actions[i].data.fd);
						break;
					case POSIX_SPAWN_DUP2:
						dup2(file_actions->actions[i].data.fd, file_actions->actions[i].data.newfd);
						break;
				}
			}
		}

		/* 3. Exec */
		execve(path, argv, envp);
		
		/* If exec fails, exit immediately. Do not return. */
		_exit(127);
	}

	/* Parent Process resumes here */
	if (pid) *pid = p;
	return 0;
}

static inline int posix_spawnp(pid_t *pid, const char *file, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
	if (strchr(file, '/')) {
		return posix_spawn(pid, file, file_actions, attrp, argv, envp);
	}

	const char *path = getenv("PATH");
	if (!path) path = "/bin:/usr/bin";

	char buf[1024];
	const char *p = path;
	while (*p) {
		const char *end = strchr(p, ':');
		if (!end) end = p + strlen(p);
		
		size_t len = end - p;
		if (len + 1 + strlen(file) + 1 > sizeof(buf)) {
			p = *end ? end + 1 : end;
			continue;
		}

		memcpy(buf, p, len);
		buf[len] = '/';
		strcpy(buf + len + 1, file);

		if (access(buf, X_OK) == 0) {
			return posix_spawn(pid, buf, file_actions, attrp, argv, envp);
		}

		p = *end ? end + 1 : end;
	}

	return ENOENT;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* _SPAWN_H */
