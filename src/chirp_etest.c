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
//    :noindex:
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
    ch_chirp_set_auto_stop(&chirp);
    ch_run(&loop);
    return ch_loop_close(&loop);
}
