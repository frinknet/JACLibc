/* (c) 2026 FRINKnet & Friends - MIT License */

/**
 * USAGE:
 *
 * In the header at the top:
 *
 * #include <config.h>
 * #define _UNISTD_LOCK JACL_SAFE
 * #define _H_LOCK JACL_SAFE
 * #include JACL_SAFE_FILE
 * #ifndef _UNISTD_H
 * #define _UNISTD_H
 *
 * ...
 *
 *
 * #endif
 * #define _H_DONE JACL_SAFE
 * #include JACL_SAFE_FILE
 * #undef _UNISTD_LOCK
 *
 * NOTE: Both JACL_SAFE and JACL_SAFE_FILE are defined in <config.h>
 * JACL_SAFE_FILE points to this file and JACL_SAFE is expanded from !__H_DEEP
 *
 * Also notice that all symbols are renamed for namespace safety and the macros
 * that hold the real names replace everything allowing for clean compilation.
 */

#ifdef  _H_LOCK
#ifdef JACL_HAS_DEBUG
#define _H_DEEP 1
#endif

#ifndef _H_DEEP

/* _STDIO_H */
#define fn __fn

#endif
#ifndef _H_DEEP // we always increment after
#define _H_DEEP 1
#elif   _H_DEEP == 1
#undef  _H_DEEP
#define _H_DEEP 2
#elif   _H_DEEP == 2
#undef  _H_DEEP
#define _H_DEEP 3
#elif   _H_DEEP == 3
#undef  _H_DEEP
#define _H_DEEP 4
#elif   _H_DEEP == 4
#undef  _H_DEEP
#define _H_DEEP 5
#elif   _H_DEEP == 5
#undef  _H_DEEP
#define _H_DEEP 6
#elif   _H_DEEP == 6
#undef  _H_DEEP
#define _H_DEEP 7
#elif   _H_DEEP == 7
#undef  _H_DEEP
#define _H_DEEP 8
#elif   _H_DEEP == 8
#undef  _H_DEEP
#define _H_DEEP 9
#elif   _H_DEEP == 9
#error "Headers nested too deep!!!"
#endif

#undef _H_LOCK
#endif

/* * * * * * * * * * * * * * * */
#ifdef _H_DONE

#if     _H_DEEP == 9
#undef  _H_DEEP
#define _H_DEEP 8
#elif   _H_DEEP == 8
#undef  _H_DEEP
#define _H_DEEP 7
#elif   _H_DEEP == 7
#undef  _H_DEEP
#define _H_DEEP 6
#elif   _H_DEEP == 6
#undef  _H_DEEP
#define _H_DEEP 5
#elif   _H_DEEP == 5
#undef  _H_DEEP
#define _H_DEEP 4
#elif   _H_DEEP == 4
#undef  _H_DEEP
#define _H_DEEP 3
#elif   _H_DEEP == 3
#undef  _H_DEEP
#define _H_DEEP 2
#elif   _H_DEEP == 2
#undef  _H_DEEP
#define _H_DEEP 1
#elif   _H_DEEP == 1
#undef  _H_DEEP
#endif

#ifndef JACL_HAS_DEBUG

#if !_STDIO_LOCK
#undef __fn
#endif /* _STDIO_LOCK */

#endif
#undef _H_DONE
#endif
