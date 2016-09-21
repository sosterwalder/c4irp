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
    include_dirs=["include", "build/libuv/include"],
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

struct ch_chirp;

typedef struct ch_connection {
    uint8_t               ip_protocol;
    uint8_t               address[16];
    int32_t               port;
    uv_tcp_t              client;
    void*                 buffer;
    size_t                buffer_size;
    struct ch_chirp*      chirp;
    uv_shutdown_t         shutdown_req;
    uint8_t               flags;
    char                  color_field;
    struct ch_connection* left;
    struct ch_connection* right;
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
