// =====================================
// Bound-checked arrays for minimal C99.
// =====================================
//
// Create a prototype checked array
//
// .. code-block:: cpp
//
//     CA_PROTOTYPE(type)
//
// If you call CA_PROTOTYPE(int) you get the type:
//
// .. code-block:: cpp
//
//     ca_int_t
//
// and the functions:
//
// .. code-block:: cpp
//
//    ca_new_int(size)
//
// Creates a new array of size
//
// .. code-block:: cpp
//
//    ca_free_int(array)
//
// Frees the array
//
// The macro CA(type, array, index) uses the ca_##type##_p function to
// magically access the array. You can use this macro like:
//
// .. code-block:: cpp
//
//    printf("%d", CA(int, a, 3));
//
// or
//
// .. code-block:: cpp
//
//    CA(int, a, 3) = 2;
//
// If NDEBUG is set all overhead will be optimized away.
//
// .. code-block:: cpp

#ifndef ca_check_array_h
#define ca_check_array_h

#include "chirp.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef NDEBUG
#define CA(type, array, index)                     \
*ca_##type##_p(                                    \
    array,                                         \
    index,                                         \
    __FILE__,                                      \
    __LINE__                                       \
)

#define CA_PROTOTYPE(type)                         \
typedef struct {                                   \
    type* data;                                    \
    size_t len;                                    \
}                                                  \
ca_##type##_t;                                     \
                                                   \
static inline ca_##type##_t                        \
ca_new_##type(                                     \
ch_chirp_t* chirp,                                 \
size_t size                                        \
) {                                                \
    ca_##type##_t array;                           \
    array.data = (type*) ch_chirp_alloc(           \
        chirp,                                     \
        size * sizeof(type)                        \
    );                                             \
    array.len = size;                              \
    return array;                                  \
}                                                  \
                                                   \
static inline ca_##type##_t                        \
ca_from_pointer_##type(                            \
    type* data,                                    \
    size_t size                                    \
) {                                                \
    ca_##type##_t array;                           \
    array.data = data;                             \
    array.len = size;                              \
    return array;                                  \
}                                                  \
                                                   \
static inline void                                 \
ca_free_##type(                                    \
    ch_chirp_t* chirp,                             \
    ca_##type##_t array                            \
) {                                                \
    ch_chirp_free(chirp, array.data);              \
}                                                  \
                                                   \
static inline type*                                \
ca_##type##_p(                                     \
    ca_##type##_t array,                           \
    size_t index,                                  \
    const char file[],                             \
    int line                                       \
) {                                                \
    if(!(index < array.len)) {                     \
        fprintf(                                   \
            stderr,                                \
            "Bufferoverflow at %s:%d\n"            \
            "\ttype:  %s\n"                        \
            "\tlen:   %zu\n"                       \
            "\tindex: %zu\n",                      \
            file, line, #type, array.len, index    \
        );                                         \
        assert(index < array.len);                 \
    }                                              \
    return &(array.data[index]);                   \
}
#else
#define CA(type, array, index)                     \
(array[index])

#define CA_PROTOTYPE(type)                         \
typedef type* ca_##type##_t;                       \
                                                   \
static inline type*                                \
ca_new_##type(                                     \
    ch_chirp_t* chirp,                             \
    size_t size                                    \
) {                                                \
    return (type*) ch_chirp_alloc(                 \
        chirp,                                     \
        size * sizeof(type)                        \
    );                                             \
}                                                  \
                                                   \
static inline void                                 \
ca_free_##type(                                    \
    ch_chirp_t* chirp,                             \
    type* array                                    \
) {                                                \
    ch_chirp_free(chirp, array);                   \
}                                                  \
                                                   \
static inline ca_##type##_t                        \
ca_from_pointer_##type(                            \
    type* data,                                    \
    size_t size                                    \
) {                                                \
    (void)(size);                                  \
    return data;                                   \
}                                                  \
                                                   \
static inline type*                                \
ca_##type##_p(                                     \
    ca_##type##_t array,                           \
    size_t index,                                  \
    const char file[],                             \
    int line                                       \
) {                                                \
    (void)(file);                                  \
    (void)(line);                                  \
    return &(array[index]);                        \
}
#endif //NDEBUG

#endif //ca_check_array_h

