from cffi import FFI

ffi = FFI()

ffi.set_source(
    "_c4irp_low_level",
    """
    #include "../c4irpc/connection_test.h"
    """,
    libraries=[
        "c4irp",
        "uv",
        "m",
        "rt",
        "pthread",
        "mbedcrypto",
        "mbedx509",
        "mbedcrypto",
    ],
    library_dirs=["."],
    include_dirs=["include"],
)


ffi.cdef("""
//connection.h
typedef struct ch_connection {
    uint8_t               ip_protocol;
    uint8_t               address[16];
    int32_t               port;
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
