"""Testing the chirp message structure."""
from _chirp_cffi import ffi, lib


def test_low_level_new():
    """Test if cffi low level new works."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    assert message.port == 0


def test_low_level_set_get_address4():
    """Test if cffi low level set and get of address works."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "49.32.12.1".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV4,
        ip_addr,
        3432,
    ) == lib.CH_SUCCESS
    assert message.port == 3432
    address = ffi.new("ch_text_address_t*")
    assert lib.ch_msg_get_address(
        message,
        address
    ) == lib.CH_SUCCESS
    assert ffi.string(address.data) == ip_addr


def test_low_level_set_get_address4_error():
    """Test if cffi low level set and get of address errors are correct."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "349.32.12.1".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV4,
        ip_addr,
        3432,
    ) == lib.CH_VALUE_ERROR


def test_low_level_set_get_address4_bad_proto():
    """Test if cffi low level set and get of address errors are correct."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "49.32.12.1".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        100,
        ip_addr,
        3432,
    ) == lib.CH_VALUE_ERROR


def test_low_level_set_get_address4_error_back():
    """Test if cffi low level set and get of address errors are correct."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "49.32.12.1".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV4,
        ip_addr,
        3432,
    ) == lib.CH_SUCCESS
    assert message.port == 3432
    address = ffi.new("ch_text_address_t*")
    message.ip_protocol = 3
    assert lib.ch_msg_get_address(
        message,
        address
    ) == lib.CH_PROTOCOL_ERROR


def test_low_level_set_get_address6():
    """Test if cffi low level set and get of address works."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "2001:db8:85a3:0:0:8a2e:370:7334".encode("UTF-8")
    res = "2001:db8:85a3::8a2e:370:7334".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV6,
        ip_addr,
        3432,
    ) == lib.CH_SUCCESS
    assert message.port == 3432
    address = ffi.new("ch_text_address_t*")
    assert lib.ch_msg_get_address(
        message,
        address
    ) == lib.CH_SUCCESS
    assert ffi.string(address.data) == res


def test_low_level_set_get_address6_error():
    """Test if cffi low level set and get of address errors are correct."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "200x:db8:85a3:0:0:8a2e:370:7334".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV6,
        ip_addr,
        3432,
    ) == lib.CH_VALUE_ERROR


def test_low_level_set_get_address6_error_back():
    """Test if cffi low level set and get of address errors are correct."""
    message = ffi.new("ch_message_t*")
    lib.ch_msg_init(message)
    ip_addr = "2001:db8:85a3:0:0:8a2e:370:7334".encode("UTF-8")
    assert message.port == 0
    assert lib.ch_msg_set_address(
        message,
        lib.CH_IPV6,
        ip_addr,
        3432,
    ) == lib.CH_SUCCESS
    assert message.port == 3432
    address = ffi.new("ch_text_address_t*")
    message.ip_protocol = 3
    assert lib.ch_msg_get_address(
        message,
        address
    ) == lib.CH_PROTOCOL_ERROR
