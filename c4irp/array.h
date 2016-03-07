/* Bound-checked arrays for minimal C99.
 *
 * CA_PROTOTYPE(type) : Create a prototype checked array.
 *
 * If you call CA_PROTOTYPE(int) you get the type:
 *
 * ca_int_type
 *
 * and the functions:
 *
 * ca_new_int(size): Creates a new array of size
 * ca_free_int(array): Frees the array
 *
 * The macro CA(type, array, index) uses the ca_##type##_p function to
 * magically access the array. You can use this macro like:
 *
 * printf("%d", CA(int, a, 3));
 *
 * or
 *
 * CA(int, a, 3) = 2;
 *
 * If NDEBUG is set all overhead will be optimized away.
 *
 * */

#ifndef ca_check_array_h
#define ca_check_array_h

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
ca_##type##_type;                                  \
                                                   \
static inline ca_##type##_type                     \
ca_new_##type(                                     \
    size_t size                                    \
) {                                                \
    ca_##type##_type array;                        \
    array.data = (type*) malloc(                   \
        size * sizeof(type)                        \
    );                                             \
    array.len = size;                              \
    return array;                                  \
}                                                  \
                                                   \
static inline ca_##type##_type                     \
ca_from_pointer_##type(                            \
    type* data,                                    \
    size_t size                                    \
) {                                                \
    ca_##type##_type array;                        \
    array.data = data;                             \
    array.len = size;                              \
    return array;                                  \
}                                                  \
                                                   \
static inline void                                 \
ca_free_##type(                                    \
    ca_##type##_type array                         \
) {                                                \
    free(array.data);                              \
}                                                  \
                                                   \
static inline type*                                \
ca_##type##_p(                                     \
    ca_##type##_type array,                        \
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
        assert(0);                                 \
    }                                              \
    return &(array.data[index]);                   \
}
#else
#define CA(type, array, index)                     \
(array[index])

#define CA_PROTOTYPE(type)                         \
typedef type* ca_##type##_type;                    \
                                                   \
static inline type*                                \
ca_new_##type(                                     \
    size_t size                                    \
) {                                                \
    return (type*) malloc(size * sizeof(type));    \
}                                                  \
                                                   \
static inline void                                 \
ca_free_##type(                                    \
    type* array                                    \
) {                                                \
    free(array);                                   \
}                                                  \
                                                   \
static inline ca_##type##_type                     \
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
    ca_##type##_type array,                        \
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

