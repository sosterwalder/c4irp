// ===================
// Testing basic chirp
// ===================
//
// .. code-block:: cpp

#include "../include/chirp.h"


// .. c:function::
int
main(
    int argc,
    char *argv[]
)
//
//    Run chirp
//
// .. code-block:: cpp

{
    ch_chirp_t chirp;
    uv_loop_t loop;
    ch_config_t config = ch_config_defaults;
    ch_loop_init(&loop);
    ch_chirp_init(&chirp, &config, &loop);
    ch_run(&loop);
    ch_loop_close(&loop);
    return 0;
}
