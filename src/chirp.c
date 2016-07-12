// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "../config.h"
#include "common.h"
#include "chirp.h"
#include "message.h"  // TODO remove?
#include "protocol.h"

#include <uv.h>

#include <limits.h>
#include <stdlib.h>

//
// .. c:var:: ch_config_defaults
//
//    Default config of chirp.
//
// .. code-block:: cpp
//
ch_config_t ch_config_defaults = {
    .REUSE_TIME     = 30,
    .TIMEOUT        = 5,
    .PORT           = 2998,
    .BACKLOG        = 100,
    .BIND_V6        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4        = {0, 0, 0, 0},
    .CERT_CHAIN_PEM = NULL,
    .ALLOC_CB       = NULL,
    .FREE_CB        = NULL,
};

// .. c:function::
void
_ch_close_async_cb(uv_async_t* handle)
//
//    Internal callback to close chirp. Makes ch_chirp_close_ts thread-safe
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_chirp_t* chirp = (ch_chirp_t*) handle->data;
    ch_chirp_int_t* ichirp = chirp->_;
    tmp_err = ch_pr_stop(&ichirp->protocol);
    A(tmp_err == CH_SUCCESS, "Closing failed with error %d", tmp_err);
    (void)(tmp_err);
    uv_close((uv_handle_t*) &ichirp->close, NULL);
    if(ichirp->auto_start) {
        uv_stop(chirp->loop);
        L(chirp, "UV-Loop %p stopped by chirp", chirp->loop);
    }
    L(chirp, "Closed chirp %p", chirp);
}

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t* config, uv_loop_t* loop)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    int                       tmp_err;
    ch_chirp_int_t* ichirp  = malloc(sizeof(ch_chirp_int_t));
    ch_protocol_t* protocol = &ichirp->protocol;
    chirp->_                = ichirp;
    chirp->loop             = loop;
    chirp->config           = config;
    ichirp->auto_start      = 0;
    chirp->_log             = NULL;

    // rand
    srand(time(NULL));
    _ch_random_ints_to_bytes(chirp->identity, 16);

    if(uv_async_init(chirp->loop, &ichirp->close, &_ch_close_async_cb) < 0) {
        return CH_UV_ERROR; // NOCOV
    }

    protocol->chirp    = chirp;
    tmp_err            = ch_pr_start(protocol);
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    chirp->_init = CH_CHIRP_MAGIC;
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_run(ch_config_t* config, ch_chirp_t** chirp_out)
//    :noindex:
//
//    see: :c:func:`ch_chirp_run`
//
// .. code-block:: cpp
//
{
    ch_chirp_t chirp;
    uv_loop_t  loop;
    ch_error_t tmp_err;
    if(chirp_out != NULL) {
        *chirp_out = &chirp;
    }

    tmp_err = _ch_uv_error_map(ch_loop_init(&loop));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;  // NOCOV this can only fail with access error
    }
    tmp_err = ch_chirp_init(&chirp, config, &loop);
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;  // NOCOV covered in ch_chirp_init tests
    }
    chirp._->auto_start = 1;
    L((&chirp), "UV-Loop %p run by chirp", &loop);
    tmp_err = _ch_uv_error_map(ch_run(&loop, UV_RUN_DEFAULT));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;  // NOCOV only breaking things will trigger this
    }
    if(ch_loop_close(chirp.loop)) {
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_ts`
//
//    This function is thread-safe
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Chirp not initialized");
    ch_chirp_int_t* ichirp = chirp->_;
    ichirp->close.data = chirp;
    if(uv_async_send(&ichirp->close) < 0) {
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
}
