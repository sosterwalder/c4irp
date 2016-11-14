"""Common variables for cffi derived from platform and MODE."""
import sys
import os

libs = [
    "chirp",
    "uv",
]

cflags = []
ldflags = []

if 'MODE' not in os.environ:
    os.environ['MODE'] = "release"

if sys.platform == "win32":
    libs.extend([
        "advapi32",
        "iphlpapi",
        "psapi",
        "shell32",
        "user32",
        "userenv",
        "ws2_32",
        "kernel32",
        "libeay32",
        "ssleay32",
        "gdi32",
    ])
    ldflags.extend(["/LIBPATH:openssl\\lib"])
    if os.environ['MODE'].lower() == "debug":
        cflags.extend(["/Zi", "/Od", "/DEBUG"])
        ldflags.extend(["/DEBUG"])
else:
    libs.extend([
        "m",
        "pthread",
        "ssl",
        "crypto",
    ])
    cflags.append("-std=gnu99")
    if sys.platform != "darwin":
        libs.append("rt")
    else:
        cflags.append("-I/usr/local/opt/openssl/include")
        ldflags.append("-L/usr/local/opt/openssl/lib")
    if os.environ['MODE'] == "debug":
        cflags.extend(["--coverage", "-UNDEBUG", "-O0"])
        ldflags.extend(["--coverage"])
