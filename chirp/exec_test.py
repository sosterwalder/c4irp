"""Running test executables."""
# Start ignoring PyLintBear*E1101*

from . import sh

array = sh.command("src/array_etest")
ssl_client = sh.command("src/basic_ssl_client_etest")
ssl_server = sh.background_command("src/basic_ssl_server_etest")


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
