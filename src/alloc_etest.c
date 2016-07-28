// ==================
// Testing allocators
// ==================
//
// .. code-block:: cpp

#include "../include/chirp.h"
#include <stdlib.h>

// .. c:function::
static
void*
bad_alloc(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
)
//
//    A bad allocator to test if it causes asserts.
//
// .. code-block:: cpp
//
{
    *provided_size = suggested_size - 1; // NOCOV not testable since it segfaults
    return malloc(*provided_size); // NOCOV not testable since it segfaults
}

// .. c:function::
int
main(
    int argc,
    char *argv[]
)
//
//    Test if a bad memory allocator will assert.
//
//    :param int argc: Argument count
//    :param char\*\* argv: List of arguments
//
// .. code-block:: cpp

{
    int bad = strtol(argv[1], NULL, 10);
    ch_config_t config = ch_config_defaults;
    if(bad) {
        config.ALLOC_CB = bad_alloc; // NOCOV not testable since it segfaults
    } // NOCOV not testable since it segfaults
    ch_chirp_t chirp;
    uv_loop_t loop;
    ch_loop_init(&loop);
    ch_chirp_init(&chirp, &config, &loop);
}
