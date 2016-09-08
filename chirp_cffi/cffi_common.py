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
    ])
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
    if sys.platform != "darwin":
        libs.append("rt")
    else:
        cflags.append("-I/usr/local/opt/openssl/include")
        ldflags.append("-L/usr/local/opt/openssl/lib")
    if os.environ['MODE'] == "debug":
        cflags.extend(["--coverage"])
        ldflags.extend(["--coverage"])
