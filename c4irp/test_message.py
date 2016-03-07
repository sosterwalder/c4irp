from ._high_level import ffi, lib


def test_low_level_new():
    """Test if cffi low level new works"""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    assert message.port == 0
    assert message.host_order == 1
