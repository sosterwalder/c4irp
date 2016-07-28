"""Running test executables."""

import sh

array = sh.Command("src/array_etest")
alloc = sh.Command("src/alloc_etest")


def test_alloc_good():
    alloc(0)


def test_alloc_bad():
    try:
        alloc(1)
    except sh.SignalException_6 as e:
        assert b"Not enough memory provided by ALLOC_CB" in e.stderr


def test_array_overflow():
    try:
        array(3)
    except sh.SignalException_6 as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_underflow():
    try:
        array(-1)
    except sh.SignalException_6 as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_ok():
    array(1)
