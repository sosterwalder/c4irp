import glob
import subprocess
import os

cflags = []
if "CFLAGS" in os.environ:
    cflags = os.environ["CFLAGS"].split(" ")

sources = set([x[:-2] for x in glob.glob("src/*.c")])
etests = set([x for x in sources if x.endswith("_etest")])
tests = set([x for x in sources if x.endswith("_test")])
objs = sources - etests - tests

try:
    for source in sources:
        cmd = ["cl"] + cflags + ["/c", "%s.c" % source]
        print(subprocess.check_output(cmd))

except subprocess.CalledProcessError as e:
    print(e.output)
    raise e
