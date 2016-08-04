import glob
import subprocess
import os

cflags = []
if "CFLAGS" in os.environ:
    cflags = os.environ["CFLAGS"].split(" ")

sources = set([x[:-2] for x in glob.glob("src/*.c")])
etests = set([x for x in sources if x.endswith("_etest")])
tests = set([x for x in sources if x.endswith("_test")])
objs = sources - etests

if os.environ["config"] == "Release":
    objs = objs - tests


def exec_cmd(cmd):
    """Execute a compiler command"""
    print(" ".join(cmd))
    print(subprocess.check_output(cmd))

try:
    for source in sources:
        cmd = ["cl"] + cflags + ["/c", "%s.c" % source, "/Fo%s.obj" % source]
        exec_cmd(cmd)
    cmd = ["lib", "/nologo", "/LTCG"] + [
        "%s.obj" % x for x in objs
    ] + ["/out:chirp.lib"]
    exec_cmd(cmd)
except subprocess.CalledProcessError as e:
    print(e.output)
    raise e
