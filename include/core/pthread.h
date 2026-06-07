/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _CORE_PTHREAD_H
#define _CORE_PTHREAD_H
#pragma once

#include <config.h>
#include <pthread.h>

#if PTHREAD_POSIX

_Atomic unsigned int __jacl_pthread_tls_key_counter = 0;
thread_local void *__jacl_pthread_tls_values[MAX_TLS_KEYS] = {0};

void (*__jacl_pthread_key_destructors[MAX_TLS_KEYS])(void *) = {0};

pthread_key_t __jacl_pthread_self_key = 0;
_Atomic int __jacl_pthread_inited = 0;

__jacl_atfork_handler_t *__jacl_atfork_list = NULL;
pthread_spinlock_t __jacl_atfork_lock = PTHREAD_SPIN_LOCK_INITIALIZER;

#endif

#endif /* _CORE_PTHREAD_H */
