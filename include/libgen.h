/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _LIBGEN_H
#define _LIBGEN_H

#pragma once

#include <config.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int splitname(char *path, char **dir, char **base) {
    if (!path || !dir || !base) return -1;

    char *p = path;
    char *end = p;
    while (*end) end++;

    if (end == p) {
        *dir = ".";
        *base = ".";
        return 0;
    }

    char *last = end - 1;
    while (last >= p && *last == '/')
        last--;

    if (last < p) {
        *dir = "/";
        *base = "/";
        return 0;
    }

    char *sep = last;
    while (sep > p && sep[-1] != '/')
        sep--;

    if (sep > p && sep[-1] == '/') {
        sep[-1] = '\0';
        *(last + 1) = '\0';
        *base = sep;
        *dir = p;

        char *k = sep - 1;
        while (k > p && *(k - 1) == '/')
            k--;
        
        *k = '\0';
        
        if (*p == '\0') *dir = "/";

    } else {
        *(last + 1) = '\0';
        *dir = ".";
        *base = p;
    }
    return 0;
}

char *dirname(char *path) {
    char *dir, *base;
    if (splitname(path, &dir, &base) < 0) return ".";
    return dir;
}

char *basename(char *path) {
    char *dir, *base;
    if (splitname(path, &dir, &base) < 0) return ".";
    return base;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBGEN_H */
