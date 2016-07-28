"""Running test executables."""

import sh

array = sh.Command("src/array_etest")


def test_array_overflow():
    try:
        array(3, 2)
    except sh.SignalException_6 as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_underflow():
    try:
        array(-1, 2)
    except sh.SignalException_6 as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_ok():
    array(1, 3)
