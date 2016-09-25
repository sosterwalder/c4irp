"""Testing the chirp object."""
import threading
import time
from contextlib import contextmanager

from hypothesis import strategies as st
from hypothesis import given

from _chirp_cffi import ffi, lib

from . import ChirpPool

config_st = st.fixed_dictionaries({
    "REUSE_TIME"   : st.integers(2, 3600),
    "TIMEOUT"      : st.floats(0.1, 60),
    "IP_PROTOCOLS" : st.integers(0, 3),
    "BACKLOG"      : st.integers(0, 1000),
    "RETRIES"      : st.integers(0, 4),
    "DEBUG"        : st.booleans(),
    "MAX_HANDLERS" : st.integers(1, 100),
    # TODO in python chirp MAX_HANDLERS arent validated complete
    # add this to cccirp and chirp
    "REQUIRE_ACK"  : st.booleans(),
    "FLOW_CONTROL" : st.booleans(),
    "RESOLVE"      : st.booleans(),
    "DISABLE_TASKS": st.booleans(),
    "DISABLE_TLS"  : st.booleans(),
    "LOCALHOST_OPT": st.booleans(),
})


def test_init_free():
    """Test if we can initialize and free chirp."""
    with basic_uv() as error:
        assert error == lib.CH_SUCCESS


def test_init_bind_err():
    """Test if we create a bind error."""
    with basic_uv() as error:
        assert error == lib.CH_SUCCESS
        with basic_uv() as error2:
            assert error2 == lib.CH_EADDRINUSE


@contextmanager
def basic_uv():
    """Create a basic libuv context."""
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    error = lib.ch_chirp_init(
        chirp, ffi.addressof(lib.ch_config_defaults), loop
    )
    lib.ch_chirp_set_auto_stop(chirp)
    lib.ch_chirp_register_log_cb(chirp, lib.python_log_cb)
    yield error
    if error == lib.CH_SUCCESS:
        assert lib.ch_chirp_close_ts(chirp) == lib.CH_SUCCESS
        assert lib.ch_run(loop) == lib.CH_SUCCESS
        assert lib.ch_loop_close(loop) == lib.CH_SUCCESS


def test_init_bad_port_low():
    """Test if init with bad port throws CH_VALUE_ERROR."""
    init_bad_port(-123)


def test_init_bad_port_high():
    """Test if init with bad port throws CH_VALUE_ERROR."""
    init_bad_port(65536)


def init_bad_port(port):
    """Test if init with bad port throws CH_VALUE_ERROR."""
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    config = ffi.new("ch_config_t*")
    config[0] = lib.ch_config_defaults
    assert config.PORT == 2998
    config.PORT = port
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    assert lib.ch_chirp_init(
        chirp, config, loop
    ) == lib.CH_VALUE_ERROR


def test_chirp_run():
    """Test if the do everything ch_chirp_run method works."""
    res = []
    chirp_p = ffi.new("ch_chirp_t**")

    def run():
        """Run chirp in a thread."""
        res.append(lib.ch_chirp_run(
            ffi.addressof(lib.ch_config_defaults), chirp_p
        ))

    thread = threading.Thread(target=run)
    thread.start()
    time.sleep(0.1)
    lib.ch_chirp_close_ts(chirp_p[0])
    thread.join()
    assert res[0] == lib.CH_SUCCESS


@given(config_st)
def test_chirp_object_config(config):
    """Test if initializing, using and closing the ChirpPool object works...

    with hypothesis generated config.
    """
    # TODO as validate is implemented this is going to need assume()s
    chirp = init_chirp(config)
    # TODO send a message to second (standard c chirp)
    chirp.close()


def test_chirp_object_basic():
    """Test if initializing and closing the ChirpPool object works."""
    chirp = init_chirp()
    chirp.close()


def test_chirp_server_handshake():
    """Test if the chirp server handshake works using a test programm."""
    c = {
        'PORT': 4433,
    }
    chirp = init_chirp(c)
    chirp.close()

# TODO use pytest markers + get request


def init_chirp(c=None):
    """Initialize chirp for basic tests."""
    if c is None:
        chirp = ChirpPool()
    else:
        chirp = ChirpPool(c)
    chirp._chirp.identity = [0] * 16
    chirp._chirp.loop = ffi.NULL
    assert chirp._chirp.loop == ffi.NULL
    chirp.start()
    assert ffi.string(chirp._chirp.identity) != b''
    assert chirp._chirp.loop != ffi.NULL
    return chirp

if __name__ == "__main__":  # pragma: no cover
    import sys
    import logging
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
    chirpobj = init_chirp()
    sys.stdin.readline()
    print("Minimal test: closing")
    chirpobj.close()
    print("Minimal test: done")
