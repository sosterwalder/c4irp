from cffi import FFI
from chirp_cffi.cffi_common import libs

ffi = FFI()

ffi.set_source(
    "_chirp_cffi",
    """
    #include <chirp.h>
    """,
    libraries=libs,
    library_dirs=["."],
    include_dirs=["include", "libuv/include"],
)


ffi.cdef("""
//uv.h
typedef enum {
    UV_RUN_DEFAULT = 0,
    UV_RUN_ONCE,
    UV_RUN_NOWAIT
} uv_run_mode;

struct uv_loop_s {
  /* User data - use this for whatever. */
  void* data;
  /* Loop reference counting. */
  unsigned int active_handles;
  void* handle_queue[2];
  void* active_reqs[2];
  /* Internal flag to signal loop stop. */
  unsigned int stop_flag;
  ...;
};
typedef struct uv_loop_s uv_loop_t;

//const.h
typedef enum {
    CH_IPV4     = 0,
    CH_IPV6     = 1
} ch_ip_protocol_t;

//error.h
typedef enum {
    CH_SUCCESS        = 0,
    CH_VALUE_ERROR    = 1,
    CH_UV_ERROR       = 2,
    CH_PROTOCOL_ERROR = 3,
    CH_EADDRINUSE     = 4,
} ch_error_t;

//message.h
typedef struct {
    char data[...];
} ch_text_address_t;

typedef struct {
    char     identity[16];
    char     serial[16];
    int8_t   message_type;
    int16_t  header_len;
    int16_t  actor_len;
    int32_t  data_len;
    char*    header;
    char*    actor;
    char*    data;
    uint8_t  ip_protocol;
    uint8_t  address[16];
    int32_t  port;
    int8_t   host_order;
    int8_t   free_header;
    int8_t   free_actor;
    int8_t   free_data;
} ch_message_t;

extern
ch_error_t
ch_msg_init(ch_message_t* message);

extern
ch_error_t
ch_msg_set_address(
    ch_message_t* message,
    ch_ip_protocol_t ip_protocol,
    const char* address,
    int32_t port
);

extern
ch_error_t
ch_msg_get_address(
    const ch_message_t* message,
    ch_text_address_t* address
);

//chirp.h
typedef void* (*ch_alloc_cb)(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
);
typedef void (*ch_free_cb)(void* buf);

typedef struct {
    int          REUSE_TIME;
    float        TIMEOUT;
    int          PORT;
    int          BACKLOG;
    char         BIND_V6[16];
    char         BIND_V4[4];
    char*        CERT_CHAIN_PEM;
    ch_alloc_cb  ALLOC_CB;
    ch_free_cb   FREE_CB;
} ch_config_t;

typedef void (*ch_log_cb_t)(char msg[]);
extern "Python" void python_log_cb(char msg[]);

extern ch_config_t ch_config_defaults;

typedef struct {
    unsigned char identity[16];
    uv_loop_t*    loop;
    ch_config_t*  config;
    ...;
} ch_chirp_t;

static
inline
void
ch_chirp_register_log_cb(ch_chirp_t* chirp, ch_log_cb_t log_cb);

static
inline
int
ch_loop_init(uv_loop_t* loop);

static
inline
int
ch_loop_close(uv_loop_t* loop);

static
inline
int
ch_run(uv_loop_t* loop, uv_run_mode mode);

extern
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t* config, uv_loop_t* loop);

extern
ch_error_t
ch_chirp_run(ch_config_t* config, ch_chirp_t**);

extern
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp);
""")


if __name__ == "__main__":
    ffi.compile()
