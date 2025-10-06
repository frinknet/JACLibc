/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDIO_H
#define CORE_STDIO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define the actual FILE structures
static FILE __stdin_file  = {0, NULL, NULL, NULL, 0, 0, 0, 0, NULL};
static FILE __stdout_file = {0, NULL, NULL, NULL, 0, 1, 0, 0, NULL};
static FILE __stderr_file = {0, NULL, NULL, NULL, 0, 2, 0, 0, NULL};

// Define the global pointers applications use
FILE* stdin  = &__stdin_file;
FILE* stdout = &__stdout_file;
FILE* stderr = &__stderr_file;

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDIO_H */
