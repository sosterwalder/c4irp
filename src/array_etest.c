// ===============
// Testing array.h
// ===============
//
// .. code-block:: cpp

#include "array.h"

// Create the prototype for a integer list.
//
// .. code-block:: cpp

CA_PROTOTYPE(int)

// .. c:function::
int
main(
    int argc,
    char *argv[]
)
//    :noindex:
//
//    The test function will trigger the bound check if called with 3+.
//
//    :param int argc: Argument count
//    :param char\*\* argv: List of arguments
//
// .. code-block:: cpp

{
    int index = strtol(argv[1], NULL, 10);
    int ret;
    ch_config_t config = ch_config_defaults;
    ch_chirp_t chirp;
    uv_loop_t loop;
    ch_loop_init(&loop);
    ch_chirp_init(&chirp, &config, &loop, NULL);
    ca_int_t a;
    a = ca_new_int(&chirp, 3);
    CA(int, a, index) = 0;
    ret = CA(int, a, index);
    ca_free_int(&chirp, a);
    return ret;
}
