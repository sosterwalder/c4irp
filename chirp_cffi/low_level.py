"""Defines the low-level bindings of libchirp used for testing."""
from cffi import FFI
from chirp_cffi.cffi_common import libs, cflags, ldflags

ffi = FFI()

ffi.set_source(
    "_chirp_low_level",
    """
    #include "../src/connection_test.h"
    """,
    libraries=libs,
    library_dirs=["."],
    include_dirs=["include", "build/libuv/include", "openssl/include"],
    extra_compile_args=cflags,
    extra_link_args=ldflags,
)


ffi.cdef("""
//connection.h

struct uv_tcp_s {
    ...;
};
typedef struct uv_tcp_s uv_tcp_t;

struct uv_shutdown_s {
    ...;
};
typedef struct uv_shutdown_s uv_shutdown_t;
struct uv_timer_s {
    ...;
};
typedef struct uv_timer_s uv_timer_t;

struct ch_chirp_s {
    ...;
};
typedef struct ch_chirp_s ch_chirp_t;

struct uv_buf_s {
    ...;
};
typedef struct uv_buf_s uv_buf_t;

struct uv_write_s {
    ...;
};
typedef struct uv_write_s uv_write_t;

typedef void (*uv_write_cb)(uv_write_t* req, int status);

struct SSL_s {
    ...;
};
typedef struct SSL_s SSL;

struct BIO_s {
    ...;
};
typedef struct BIO_s BIO;

struct ch_reader_s {
    ...;
};
typedef struct ch_reader_s ch_reader_t;

struct ch_writer_s {
    ...;
};
typedef struct ch_writer_s ch_writer_t;

typedef char ch_buf;

typedef struct ch_connection_s {
    uint8_t                 ip_protocol;
    uint8_t                 address[16];
    int32_t                 port;
    uint8_t                 remote_identity[16];
    float                   max_timeout;
    uv_tcp_t                client;
    ch_buf*                 buffer_uv;
    ch_buf*                 buffer_wtls;
    ch_buf*                 buffer_rtls;
    uv_buf_t                buffer_uv_uv;
    uv_buf_t                buffer_wtls_uv;
    uv_buf_t                buffer_any_uv;
    size_t                  buffer_size;
    uv_write_cb             write_callback;
    size_t                  write_written;
    size_t                  write_size;
    ch_buf*                 write_buffer;
    ch_chirp_t*             chirp;
    uv_shutdown_t           shutdown_req;
    uv_write_t              write_req;
    uv_timer_t              shutdown_timeout;
    int8_t                  shutdown_tasks;
    uint8_t                 flags;
    SSL*                    ssl;
    BIO*                    bio_ssl;
    BIO*                    bio_app;
    int                     tls_handshake_state;
    float                   load;
    ch_reader_t             reader;
    ch_writer_t             writer;
    char                    color_field;
    struct ch_connection_s* left;
    struct ch_connection_s* right;
} ch_connection_t;

//connection_test.h
extern
void
test_ch_cn_conn_dict(
        ch_connection_t* x,
        ch_connection_t* y,
        int* cmp,
        int* was_inserted,
        int* len,
        int* x_mem,
        int* y_mem
);
""")


if __name__ == "__main__":
    ffi.compile()
