// =============
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
#   if defined(_MSC_VER) && _MSC_VER < 1600
#       include <stdint-msvc2008.h>
#       define ch_inline __inline
#   else // _MSC_VER
#       include <stdint.h>
#       define ch_inline inline
#   endif // _MSC_VER

#   if defined(_MSC_VER) && _MSC_VER < 1900

#       define snprintf c99_snprintf
#       define vsnprintf c99_vsnprintf

        __inline int c99_vsnprintf(
                char *outBuf,
                size_t size,
                const char *format,
                va_list ap
        )
        {
            int count = -1;

            if (size != 0)
                count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
            if (count == -1)
                count = _vscprintf(format, ap);

            return count;
        }

        __inline int c99_snprintf(
                char *outBuf,
                size_t size,
                const char *format,
                ...
        )
        {
            int count;
            va_list ap;

            va_start(ap, format);
            count = c99_vsnprintf(outBuf, size, format, ap);
            va_end(ap);

            return count;
        }

#   endif // MSVC
#else // _WIN32
#   define ch_inline inline
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
#   ifdef CH_LOG_TO_STDERR
#       define L(chirp, message, ...) fprintf( \
            stderr, \
            "%s:%d " message "\n", \
            __FILE__, \
            __LINE__, \
            ##__VA_ARGS__ \
        )
#   else  //CH_LOG_TO_STDERR
#       define L(chirp, message, ...) do { \
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
#   endif
#   define A(condition, ...) do { \
        if(!(condition)) { \
            fprintf(stderr, ##__VA_ARGS__); \
            fprintf(stderr, "\n"); \
            assert(condition); \
        } \
    } while(0)
#else //NDEBUG
#   define L(chrip, message, ...) (void)(chirp); (void)(message)
#   define A(condition, ...) (void)(condition)
#endif

#ifndef A
#   error Assert macro not defined
#endif
#ifndef L
#   error Log macro not defined
#endif

#define CH_CHIRP_MAGIC 42429

#define CH_GET_CHIRP(handle) \
ch_chirp_t* chirp = (ch_chirp_t*) handle->data; \
A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*")
/* Fail fast, best I know :( */

#endif //ch_common_h
