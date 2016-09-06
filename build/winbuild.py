"""Building c4irp on windows."""
import codecs
import glob
import subprocess
import os
import sys

from chirp_cffi.cffi_common import libs, ldflags

c99 =  sys.version_info > (3, 4)

cflags = []
if "CFLAGS" in os.environ:
    cflags = os.environ["CFLAGS"].split(" ")

c99conv = os.path.join("build", "c99conv.exe")

sources = {x[:-2] for x in glob.glob("src/*.c")}
etests = {x for x in sources if x.endswith("_etest")}
tests = {x for x in sources if x.endswith("_test")}
objs = sources - etests

print(os.environ["config"])

if os.environ["config"] == "Release":
    objs = objs - tests


def exec_cmd(cmd):
    """Execute a compiler command."""
    print(" ".join(cmd))
    print(subprocess.check_output(cmd))

try:
    for source in sources:
        if c99:
            cmd = ["cl", "/TC"] + cflags + [
                "/c", "%s.c" % source, "/Fo%s.obj" % source
            ]
            exec_cmd(cmd)
        else:
            base = os.path.basename(source)
            cmd = ["cl", "/P"] + cflags + ["%s.c" % source]
            exec_cmd(cmd)
            try:
                os.unlink("%s.i" % source)
            except WindowsError:
                pass
            with codecs.open("%s.i" % source, 'w', "UTF-8") as fout:
                with codecs.open("%s.i" % base, 'r', "UTF-8") as fin:
                    for line in fin.readlines():
                        sline = line.strip('\r\n')
                        if sline:
                            if sline[-1] == '\\':
                                fout.write(sline[:-1])
                                continue
                        fout.write(line)
            cmd = [c99conv, "-ms", "%s.i" % source, "%s.conv.i" % source]
            exec_cmd(cmd)
            cmd = ["cl", "/TC"] + cflags + [
                "/c", "%s.conv.i" % source, "/Fo%s.obj" % source
            ]
            exec_cmd(cmd)
            try:
                os.unlink("%s.i" % base)
            except WindowsError:
                pass
    cmd = ["lib", "/nologo"] + [
        "%s.obj" % x for x in objs
    ] + ["/out:chirp.lib"]
    exec_cmd(cmd)
    if os.environ['MODE'].lower() == "debug":
        for test in etests:
            cmd = ["link", "/nologo"] + ldflags + [
                "/out:%s.exe" % test, "%s.obj" % test
            ] + [
                "%s.obj" % x for x in objs
            ] +  [
                "%s.lib" % lib for lib in libs
            ]
            exec_cmd(cmd)
except subprocess.CalledProcessError as e:
    print(e.output)
    raise e
