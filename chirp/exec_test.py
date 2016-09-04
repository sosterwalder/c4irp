"""Running test executables."""
# Start ignoring PyLintBear*E1101*

from . import sh


array = sh.command("src/array_etest")
alloc = sh.command("src/alloc_etest")


def test_alloc_good():
    """Test if normal alloc doesn't fail."""
    alloc(0)


def test_alloc_bad():
    """Test if a bad alloc causes an assert to fail."""
    try:
        alloc(1)
    except sh.CalledProcessError as e:
        assert b"Not enough memory provided by ALLOC_CB" in e.stderr


def test_array_overflow():
    """Test if a buffer overflow causes an assert to fail."""
    try:
        array(3)
    except sh.CalledProcessError as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_underflow():
    """Test if a buffer underflow causes an assert to fail."""
    try:
        array(-1)
    except sh.CalledProcessError as e:
        assert b"Bufferoverflow" in e.stderr


def test_array_ok():
    """Test if normal array access doesn't fail."""
    array(1)
# Stop ignoring