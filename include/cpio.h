/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _CPIO_H
#define _CPIO_H

#pragma once

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define C_IRUSR   S_IRUSR
#define C_IWUSR   S_IWUSR
#define C_IXUSR   S_IXUSR

#define C_IRGRP   S_IRGRP
#define C_IWGRP   S_IWGRP
#define C_IXGRP   S_IXGRP

#define C_IROTH   S_IROTH
#define C_IWOTH   S_IWOTH
#define C_IXOTH   S_IXOTH

#define C_ISUID   S_ISUID
#define C_ISGID   S_ISGID
#define C_ISVTX   S_ISVTX

#define C_ISDIR   S_IFDIR
#define C_ISFIFO  S_IFIFO
#define C_ISREG   S_IFREG
#define C_ISBLK   S_IFBLK
#define C_ISCHR   S_IFCHR
#define C_ISLNK   S_IFLNK
#define C_ISSOCK  S_IFSOCK

#define C_ISCTG   0110000

#define MAGIC     "070707"

#ifdef __cplusplus
}
#endif

#endif /* _CPIO_H */
