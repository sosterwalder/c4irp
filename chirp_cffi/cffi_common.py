import sys
import os

libs = [
    "chirp",
    "uv",
]

if sys.platform == "win32":
    libs.extend([
        "advapi32",
        "iphlpapi",
        "psapi",
        "shell32",
        "user32",
        "userenv",
        "ws2_32"
    ])
else:
    libs.extend([
        "m",
        "pthread",
    ])
    if sys.platform != "darwin":
        libs.append("rt")

if "SETUPCFLAGS" in os.environ:
    os.environ["CFLAGS"] = os.environ["SETUPCFLAGS"]
