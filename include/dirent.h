/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef DIRENT_H
#define DIRENT_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stddef.h>
#include <errno.h>

#if defined(_WIN32)
	#define DIRENT_WIN32
	#include <windows.h>
	#include <malloc.h>
#elif defined(__wasm__)
	#define DIRENT_WASM
#else
	#define DIRENT_POSIX
	#include <unistd.h>
	#include <fcntl.h>
	#ifdef __has_include
		#if __has_include(<sys/syscall.h>)
			#include <sys/syscall.h>
		#endif
	#endif
#endif

/* ================================================================ */
/* Constants                                                        */
/* ================================================================ */

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* File type constants for d_type field */
#define DT_UNKNOWN  0   /* Unknown file type */
#define DT_FIFO     1   /* FIFO/pipe */
#define DT_CHR      2   /* Character device */
#define DT_DIR      4   /* Directory */
#define DT_BLK      6   /* Block device */
#define DT_REG      8   /* Regular file */
#define DT_LNK      10  /* Symbolic link */
#define DT_SOCK     12  /* Socket */
#define DT_WHT      14  /* Whiteout (BSD) */

/* Convert between stat st_mode and d_type */
#define IFTODT(mode)    (((mode) & 0170000) >> 12)
#define DTTOIF(dirtype) ((dirtype) << 12)

/* ================================================================ */
/* Types and Structures                                             */
/* ================================================================ */

#ifdef DIRENT_WIN32
/* ================================================================ */
/* Windows implementation with Unicode support                     */
/* ================================================================ */

typedef struct {
	HANDLE handle;              /* Windows search handle */
	WIN32_FIND_DATAW find_data; /* Windows file data (Unicode) */
	int first_call;             /* First call flag */
	wchar_t path[PATH_MAX];     /* Directory path (Unicode) */
	struct dirent current;      /* Current entry */
	long position;              /* Current position for telldir/seekdir */
	int error_code;             /* Last error code */
} DIR;

struct dirent {
	ino_t d_ino;                /* File serial number */
	unsigned char d_type;       /* File type */
	unsigned short d_reclen;    /* Record length */
	unsigned short d_namlen;    /* Name length */
	char d_name[NAME_MAX + 1];  /* Filename (UTF-8) */
};

#elif defined(DIRENT_WASM)
/* ================================================================ */
/* WebAssembly - Directory operations not supported                */
/* ================================================================ */

typedef struct {
	int dummy;
} DIR;

struct dirent {
	ino_t d_ino;
	unsigned char d_type;
	unsigned short d_reclen;
	unsigned short d_namlen;
	char d_name[NAME_MAX + 1];
};

#else
/* ================================================================ */
/* POSIX implementation                                             */
/* ================================================================ */

typedef struct {
	int fd;                     /* Directory file descriptor */
	char *buffer;               /* Buffer for getdents */
	size_t buffer_size;         /* Buffer size */
	size_t buffer_pos;          /* Current position in buffer */
	size_t buffer_end;          /* End of valid data in buffer */
	long position;              /* Current position for telldir/seekdir */
	struct dirent current;      /* Current entry */
	int error_code;             /* Last error code */
	_Atomic int lock;           /* Simple spinlock for thread safety */
} DIR;

struct dirent {
	ino_t d_ino;                /* File serial number */
	off_t d_off;                /* File offset (Linux) */
	unsigned short d_reclen;    /* Record length */
	unsigned char d_type;       /* File type */
	char d_name[NAME_MAX + 1];  /* Filename */
};

/* Linux getdents structure (different from dirent) */
struct linux_dirent {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	char d_name[];
};

#endif

/* POSIX.1-2008 addition - posix_dent structure */
struct posix_dent {
	ino_t d_ino;               /* File serial number */
	reclen_t d_reclen;         /* Length of this entry */
	unsigned char d_type;      /* File type */
	char d_name[];            /* Filename string */
};

/* ================================================================ */
/* Thread safety documentation                                     */
/* ================================================================ */

/*
 * THREAD SAFETY NOTES:
 * 
 * readdir() - Thread-safe when each thread uses its own DIR* object.
 *             NOT safe when multiple threads access the same DIR*.
 *             Use external synchronization if needed.
 * 
 * readdir_r() - Deprecated since glibc 2.24 due to buffer overflow risks.
 *               Provided for POSIX compliance but readdir() is preferred.
 * 
 * Other functions - Generally thread-safe when operating on different DIR* objects.
 */

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#ifdef DIRENT_WIN32
/* ================================================================ */
/* Windows implementation with Unicode support                     */
/* ================================================================ */

/* Convert UTF-16 to UTF-8 */
static inline int win32_wchar_to_utf8(const wchar_t *wstr, char *utf8, size_t utf8_size) {
	int result = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, (int)utf8_size, NULL, NULL);
	return result > 0 ? 0 : -1;
}

/* Convert UTF-8 to UTF-16 */
static inline int win32_utf8_to_wchar(const char *utf8, wchar_t *wstr, size_t wstr_size) {
	int result = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, (int)wstr_size);
	return result > 0 ? 0 : -1;
}

/* Convert Windows attributes to d_type */
static inline unsigned char win32_attrs_to_dtype(DWORD attrs, DWORD reparse_tag) {
	if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
		if (reparse_tag == IO_REPARSE_TAG_SYMLINK || 
		    reparse_tag == IO_REPARSE_TAG_MOUNT_POINT) {
			return DT_LNK;
		}
	}
	if (attrs & FILE_ATTRIBUTE_DIRECTORY) return DT_DIR;
	if (attrs & FILE_ATTRIBUTE_DEVICE) return DT_CHR;
	return DT_REG;
}

static inline DIR *opendir(const char *dirname) {
	if (!dirname) {
		errno = EINVAL;
		return NULL;
	}
	
	DIR *dirp = (DIR *)malloc(sizeof(DIR));
	if (!dirp) {
		errno = ENOMEM;
		return NULL;
	}
	
	/* Convert UTF-8 to UTF-16 */
	if (win32_utf8_to_wchar(dirname, dirp->path, PATH_MAX) != 0) {
		free(dirp);
		errno = EINVAL;
		return NULL;
	}
	
	/* Build search pattern: "dirname\*" */
	size_t len = wcslen(dirp->path);
	if (len >= PATH_MAX - 3) {
		free(dirp);
		errno = ENAMETOOLONG;
		return NULL;
	}
	
	if (len > 0 && dirp->path[len - 1] != L'\\' && dirp->path[len - 1] != L'/') {
		wcscat(dirp->path, L"\\");
	}
	wcscat(dirp->path, L"*");
	
	dirp->handle = INVALID_HANDLE_VALUE;
	dirp->first_call = 1;
	dirp->position = 0;
	dirp->error_code = 0;
	
	return dirp;
}

static inline struct dirent *readdir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return NULL;
	}
	
	BOOL result;
	if (dirp->first_call) {
		dirp->handle = FindFirstFileW(dirp->path, &dirp->find_data);
		if (dirp->handle == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			errno = (error == ERROR_FILE_NOT_FOUND) ? ENOENT : EACCES;
			return NULL;
		}
		dirp->first_call = 0;
		result = TRUE;
	} else {
		result = FindNextFileW(dirp->handle, &dirp->find_data);
	}
	
	if (!result) {
		DWORD error = GetLastError();
		if (error == ERROR_NO_MORE_FILES) {
			errno = 0;  /* End of directory, not an error */
		} else {
			errno = EIO;
		}
		return NULL;
	}
	
	/* Fill dirent structure */
	dirp->current.d_ino = 0;  /* Windows doesn't have inode numbers */
	dirp->current.d_type = win32_attrs_to_dtype(
		dirp->find_data.dwFileAttributes,
		dirp->find_data.dwReserved0);
	
	/* Convert filename from UTF-16 to UTF-8 */
	if (win32_wchar_to_utf8(dirp->find_data.cFileName, 
	                        dirp->current.d_name, NAME_MAX + 1) != 0) {
		errno = EILSEQ;  /* Invalid character sequence */
		return NULL;
	}
	
	size_t name_len = strlen(dirp->current.d_name);
	dirp->current.d_namlen = (unsigned short)name_len;
	dirp->current.d_reclen = (unsigned short)(sizeof(struct dirent) - NAME_MAX - 1 + name_len + 1);
	
	dirp->position++;
	return &dirp->current;
}

/* Thread-safe readdir_r (deprecated but provided for POSIX compliance) */
static inline int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result) {
	if (!dirp || !entry || !result) {
		return EINVAL;
	}
	
	*result = NULL;
	
	struct dirent *current = readdir(dirp);
	if (!current) {
		return errno;  /* errno set by readdir */
	}
	
	/* Copy to provided buffer */
	*entry = *current;  /* Struct copy */
	*result = entry;
	return 0;
}

static inline int closedir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}
	
	if (dirp->handle != INVALID_HANDLE_VALUE) {
		FindClose(dirp->handle);
	}
	free(dirp);
	return 0;
}

static inline void rewinddir(DIR *dirp) {
	if (!dirp) return;
	
	if (dirp->handle != INVALID_HANDLE_VALUE) {
		FindClose(dirp->handle);
		dirp->handle = INVALID_HANDLE_VALUE;
	}
	dirp->first_call = 1;
	dirp->position = 0;
}

static inline long telldir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}
	return dirp->position;
}

static inline void seekdir(DIR *dirp, long pos) {
	if (!dirp) return;
	
	if (pos < 0) pos = 0;
	
	/* Windows doesn't support seeking to arbitrary positions easily */
	if (pos < dirp->position) {
		rewinddir(dirp);
	}
	
	/* Read entries until we reach the desired position */
	while (dirp->position < pos) {
		if (!readdir(dirp)) break;
	}
}

static inline int dirfd(DIR *dirp) {
	(void)dirp;
	errno = ENOTSUP;
	return -1; /* Windows doesn't have directory file descriptors */
}

static inline DIR *fdopendir(int fd) {
	(void)fd;
	errno = ENOTSUP;
	return NULL; /* Not supported on Windows */
}

#elif defined(DIRENT_WASM)
/* ================================================================ */
/* WebAssembly - Directory operations not supported                */
/* ================================================================ */

static inline DIR *opendir(const char *dirname) {
	(void)dirname;
	errno = ENOSYS;
	return NULL;
}

static inline struct dirent *readdir(DIR *dirp) {
	(void)dirp;
	errno = ENOSYS;
	return NULL;
}

static inline int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result) {
	(void)dirp; (void)entry; (void)result;
	return ENOSYS;
}

static inline int closedir(DIR *dirp) {
	(void)dirp;
	errno = ENOSYS;
	return -1;
}

static inline void rewinddir(DIR *dirp) {
	(void)dirp;
}

static inline long telldir(DIR *dirp) {
	(void)dirp;
	errno = ENOSYS;
	return -1;
}

static inline void seekdir(DIR *dirp, long pos) {
	(void)dirp; (void)pos;
}

static inline int dirfd(DIR *dirp) {
	(void)dirp;
	errno = ENOSYS;
	return -1;
}

static inline DIR *fdopendir(int fd) {
	(void)fd;
	errno = ENOSYS;
	return NULL;
}

#else
/* ================================================================ */
/* POSIX implementation with improved thread safety                */
/* ================================================================ */

#define DIRENT_BUFFER_SIZE 4096

/* Simple atomic operations for spinlock */
static inline void dir_lock(DIR *dirp) {
	int expected = 0;
	while (!__atomic_compare_exchange_n(&dirp->lock, &expected, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
		expected = 0;
		/* Yield to avoid busy spinning */
		sched_yield();
	}
}

static inline void dir_unlock(DIR *dirp) {
	__atomic_store_n(&dirp->lock, 0, __ATOMIC_RELEASE);
}

static inline DIR *opendir(const char *dirname) {
	if (!dirname) {
		errno = EINVAL;
		return NULL;
	}
	
	int fd;
#if defined(SYS_open) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		fd = (int)syscall(SYS_open, dirname, O_RDONLY | O_CLOEXEC);
	#else
		fd = open(dirname, O_RDONLY | O_CLOEXEC);
	#endif
#else
	fd = open(dirname, O_RDONLY | O_CLOEXEC);
#endif
	
	if (fd == -1) return NULL;
	
	DIR *dirp = (DIR *)malloc(sizeof(DIR));
	if (!dirp) {
		close(fd);
		errno = ENOMEM;
		return NULL;
	}
	
	dirp->buffer = (char *)malloc(DIRENT_BUFFER_SIZE);
	if (!dirp->buffer) {
		close(fd);
		free(dirp);
		errno = ENOMEM;
		return NULL;
	}
	
	dirp->fd = fd;
	dirp->buffer_size = DIRENT_BUFFER_SIZE;
	dirp->buffer_pos = 0;
	dirp->buffer_end = 0;
	dirp->position = 0;
	dirp->error_code = 0;
	__atomic_store_n(&dirp->lock, 0, __ATOMIC_RELAXED);
	
	return dirp;
}

static inline struct dirent *readdir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return NULL;
	}
	
	/* Note: readdir() is NOT thread-safe when multiple threads access the same DIR* */
	/* Use external synchronization or separate DIR* objects per thread */
	
	/* Refill buffer if needed */
	if (dirp->buffer_pos >= dirp->buffer_end) {
#if defined(SYS_getdents) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		ssize_t bytes = syscall(SYS_getdents, dirp->fd, dirp->buffer, dirp->buffer_size);
	#else
		ssize_t bytes = -1;
		errno = ENOSYS;
	#endif
#else
		ssize_t bytes = -1;
		errno = ENOSYS;
#endif
		
		if (bytes <= 0) {
			if (bytes == 0) errno = 0;  /* End of directory */
			return NULL;
		}
		
		dirp->buffer_end = bytes;
		dirp->buffer_pos = 0;
	}
	
	/* Validate buffer bounds */
	if (dirp->buffer_pos + sizeof(struct linux_dirent) > dirp->buffer_end) {
		errno = EIO;
		return NULL;
	}
	
	/* Parse current entry from buffer */
	struct linux_dirent *d = (struct linux_dirent *)(dirp->buffer + dirp->buffer_pos);
	
	/* Validate record length */
	if (d->d_reclen < sizeof(struct linux_dirent) || 
	    dirp->buffer_pos + d->d_reclen > dirp->buffer_end ||
	    d->d_reclen > sizeof(struct linux_dirent) + NAME_MAX + 1) {
		errno = EIO;
		return NULL;
	}
	
	/* Fill standard dirent structure */
	dirp->current.d_ino = d->d_ino;
	dirp->current.d_off = d->d_off;
	dirp->current.d_reclen = d->d_reclen;
	
	/* Get d_type from the byte after the name */
	char *type_ptr = (char *)d + d->d_reclen - 1;
	dirp->current.d_type = *type_ptr;
	
	/* Copy name with bounds checking */
	size_t name_len = strnlen(d->d_name, d->d_reclen - sizeof(struct linux_dirent));
	if (name_len > NAME_MAX) name_len = NAME_MAX;
	
	memcpy(dirp->current.d_name, d->d_name, name_len);
	dirp->current.d_name[name_len] = '\0';
	
	/* Move to next entry */
	dirp->buffer_pos += d->d_reclen;
	dirp->position++;
	
	return &dirp->current;
}

/* Thread-safe readdir_r (deprecated but provided for POSIX compliance) */
static inline int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result) {
	if (!dirp || !entry || !result) {
		return EINVAL;
	}
	
	*result = NULL;
	
	/* Use internal locking for thread safety */
	dir_lock(dirp);
	
	struct dirent *current = readdir(dirp);
	int saved_errno = errno;
	
	if (current) {
		/* Check for potential buffer overflow */
		size_t name_len = strlen(current->d_name);
		if (name_len > NAME_MAX) {
			dir_unlock(dirp);
			return ENAMETOOLONG;
		}
		
		/* Copy to provided buffer */
		*entry = *current;  /* Struct copy */
		*result = entry;
	}
	
	dir_unlock(dirp);
	return current ? 0 : saved_errno;
}

static inline int closedir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}
	
	close(dirp->fd);
	free(dirp->buffer);
	free(dirp);
	return 0;
}

static inline void rewinddir(DIR *dirp) {
	if (!dirp) return;
	
	lseek(dirp->fd, 0, SEEK_SET);
	dirp->buffer_pos = 0;
	dirp->buffer_end = 0;
	dirp->position = 0;
}

static inline long telldir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}
	return dirp->position;
}

static inline void seekdir(DIR *dirp, long pos) {
	if (!dirp) return;
	
	if (pos < 0) pos = 0;
	
	if (pos == 0) {
		rewinddir(dirp);
		return;
	}
	
	/* Simple implementation: rewind and read forward */
	rewinddir(dirp);
	for (long i = 0; i < pos; i++) {
		if (!readdir(dirp)) break;
	}
}

static inline int dirfd(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}
	return dirp->fd;
}

static inline DIR *fdopendir(int fd) {
	if (fd < 0) {
		errno = EBADF;
		return NULL;
	}
	
	DIR *dirp = (DIR *)malloc(sizeof(DIR));
	if (!dirp) {
		errno = ENOMEM;
		return NULL;
	}
	
	dirp->buffer = (char *)malloc(DIRENT_BUFFER_SIZE);
	if (!dirp->buffer) {
		free(dirp);
		errno = ENOMEM;
		return NULL;
	}
	
	dirp->fd = fd;
	dirp->buffer_size = DIRENT_BUFFER_SIZE;
	dirp->buffer_pos = 0;
	dirp->buffer_end = 0;
	dirp->position = 0;
	dirp->error_code = 0;
	__atomic_store_n(&dirp->lock, 0, __ATOMIC_RELAXED);
	
	return dirp;
}

#endif

/* ================================================================ */
/* Additional POSIX functions (improved implementations)           */
/* ================================================================ */

/* Directory entry comparison for scandir */
static inline int alphasort(const struct dirent **a, const struct dirent **b) {
	if (!a || !*a || !b || !*b) return 0;
	return strcmp((*a)->d_name, (*b)->d_name);
}

/* Reverse alphabetical sort */
static inline int versionsort(const struct dirent **a, const struct dirent **b) {
	if (!a || !*a || !b || !*b) return 0;
	/* Simplified version sort - just use strcmp for now */
	return strcmp((*a)->d_name, (*b)->d_name);
}

/* Scan directory and sort entries (improved error handling) */
static inline int scandir(const char *dirpath, struct dirent ***namelist,
                         int (*select_fn)(const struct dirent *),
                         int (*compar)(const struct dirent **, const struct dirent **)) {
	if (!dirpath || !namelist) {
		errno = EINVAL;
		return -1;
	}
	
	DIR *dirp = opendir(dirpath);
	if (!dirp) return -1;
	
	struct dirent **list = NULL;
	int count = 0;
	int capacity = 16;
	
	list = (struct dirent **)malloc(capacity * sizeof(struct dirent *));
	if (!list) {
		closedir(dirp);
		errno = ENOMEM;
		return -1;
	}
	
	struct dirent *entry;
	while ((entry = readdir(dirp)) != NULL) {
		/* Apply filter function if provided */
		if (select_fn && !select_fn(entry)) continue;
		
		/* Resize array if needed */
		if (count >= capacity) {
			capacity *= 2;
			struct dirent **new_list = (struct dirent **)realloc(list, capacity * sizeof(struct dirent *));
			if (!new_list) {
				for (int i = 0; i < count; i++) free(list[i]);
				free(list);
				closedir(dirp);
				errno = ENOMEM;
				return -1;
			}
			list = new_list;
		}
		
		/* Allocate and copy entry */
		size_t entry_size = sizeof(struct dirent);
		list[count] = (struct dirent *)malloc(entry_size);
		if (!list[count]) {
			for (int i = 0; i < count; i++) free(list[i]);
			free(list);
			closedir(dirp);
			errno = ENOMEM;
			return -1;
		}
		
		*list[count] = *entry;  /* Struct copy */
		count++;
	}
	
	closedir(dirp);
	
	/* Sort if comparison function provided */
	if (count > 0 && compar) {
		qsort(list, count, sizeof(struct dirent *),
		      (int (*)(const void *, const void *))compar);
	}
	
	*namelist = list;
	return count;
}

/* POSIX.1-2008 getdents function */
static inline ssize_t posix_getdents(int fd, void *buf, size_t nbyte, int flags) {
	(void)flags;  /* Not used in basic implementation */
	
	if (!buf || nbyte == 0) {
		errno = EINVAL;
		return -1;
	}
	
#if defined(SYS_getdents) && defined(__has_include) && !defined(DIRENT_WIN32) && !defined(DIRENT_WASM)
	#if __has_include(<sys/syscall.h>)
		return syscall(SYS_getdents, fd, buf, nbyte);
	#endif
#endif
	
	/* Not supported or no syscall available */
	(void)fd;
	errno = ENOSYS;
	return -1;
}

/* Helper macros for name length */
#define NAMLEN(dirent) ((dirent) ? strlen((dirent)->d_name) : 0)

#ifdef __cplusplus
}
#endif
#endif /* DIRENT_H */

