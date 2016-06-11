"""General low level tests"""

from hypothesis import strategies as st
from hypothesis import given

from _c4irp_low_level import ffi, lib


@given(
    st.choices(),
    st.binary(16, 16, 16),
    st.integers(1025, 2**16),
    st.binary(16, 16, 16),
    st.integers(1025, 2**16),
    st.booleans()
)
def test_ch_cn_conn_dict(choice, address1, port1, address2, port2, force_eq):
    """Test if sglib and the connection comperator behave as expected"""
    af_inet1          = choice((2, 10))
    af_inet2          = choice((2, 10))
    if force_eq:
        af_inet2      = af_inet1
        address2      = address1
        port2         = port1
    conn1             = ffi.new("ch_connection_t*")
    conn1.ip_protocol = af_inet1
    conn1.address     = address1
    conn1.port        = port1
    conn2             = ffi.new("ch_connection_t*")
    conn2.ip_protocol = af_inet2
    conn2.address     = address2
    conn2.port        = port2
    cmp_              = ffi.new("int*")
    was_inserted      = ffi.new("int*")
    len_              = ffi.new("int*")
    x_mem             = ffi.new("int*")
    y_mem             = ffi.new("int*")
    conn1_tup         = (af_inet1, address1, port1)
    conn2_tup         = (af_inet2, address2, port2)
    equal             = conn1_tup == conn2_tup
    lib.test_ch_cn_conn_dict(
        conn1,
        conn2,
        cmp_,
        was_inserted,
        len_,
        x_mem,
        y_mem,
    )
    if equal:
        assert cmp_[0]         == 0
        assert was_inserted[0] == 0
        assert len_[0]         == 1
        assert x_mem[0]        == 1
        assert y_mem[0]        == 0
    else:
        assert cmp_[0]         != 0
        assert was_inserted[0] == 1
        assert len_[0]         == 2
        assert x_mem[0]        == 1
        assert y_mem[0]        == 1

    if conn1_tup < conn2_tup:
        assert cmp_[0] < 0
    if conn1_tup > conn2_tup:
        assert cmp_[0] > 0
