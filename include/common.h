// ===========j=
// Common header
// =============
//
// I know common headers aren't good style but Windows is forcing me.
//
// .. code-block:: cpp
//
#ifndef ch_common_h
#define ch_common_h

#include "error.h"
#include "const.h"
#include "../config.h"

#include <uv.h>

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#ifdef _WIN32
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <stdint-msvc2008.h>
#define ch_inline __inline
#else // _MSC_VER
#include <stdint.h>
#define ch_inline inline
#endif // _MSC_VER
#else // _WIN32
#define ch_inline inline
#endif // _WIN32

// Logging and assert macros
// =========================
//
// The logging macro L(chirp, message, ...) behaves like printf and allows to
// log to a custom callback. Usually used to log into pythons logging facility.
//
// The assert macro A(condition, message, ...) behaves like printf and allows to
// print a message with the assertion
//
// .. code-block:: cpp

#ifndef NDEBUG
#ifdef LOG_TO_STDERR
#define L(chirp, ...) fprintf(stderr, ##__VA_ARGS__)
#else  //LOG_TO_STDERR
#include "../include/chirp_obj.h"
#define L(chirp, message, ...) do { \
    if(chirp->_log != NULL) { \
        char buf[1024]; \
        snprintf( \
            buf, \
            1024, \
            "%s:%d " message, \
            __FILE__, \
            __LINE__, \
            ##__VA_ARGS__ \
        ); \
        chirp->_log(buf); \
    } \
} while(0)
#endif
#define A(condition, ...) do { \
    if(!(condition)) { \
        fprintf(stderr, ##__VA_ARGS__); \
        fprintf(stderr, "\n"); \
        assert(condition); \
    } \
} while(0)
#else //NDEBUG
#define L(chrip, message, ...) (void)(chirp); (void)(message)
#define A(condition, ...) (void)(condition)
#endif

#ifndef A
#error Assert macro not defined
#endif
#ifndef L
#error Log macro not defined
#endif

#define CH_CHIRP_MAGIC 42429

#define CH_GET_CHIRP(handle) \
ch_chirp_t* chirp = (ch_chirp_t*) handle->data; \
A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*")
/* Fail fast, best I know :( */

#endif //ch_common_h
