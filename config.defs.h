#ifndef ch_global_config_h
#define ch_global_config_h

//#define LOG_TO_STDERR

#ifndef NDEBUG
#ifdef LOG_TO_STDERR
#include <stdio.h>
#define L(chirp, ...) fprintf (stderr, __VA_ARGS__)
#else  //LOG_TO_STDERR
#include "include/c4irp_obj.h"
#define L(chirp, ...) do { \
    if(chirp->_log != NULL) { \
        char buf[1024]; \
        snprintf(buf, 1024, ##__VA_ARGS__); \
        chirp->_log(buf); \
    } \
} while(0)
#endif
#else //NDEBUG
#define L(...)
#endif

#endif //ch_global_config_h
