"""Creating function wrappers for shell commands."""

import subprocess
from subprocess import CalledProcessError

PIPE = subprocess.PIPE


def command(command_line):
    """Create a function wrapper for a command."""
    def func(*args):
        """Wrapper function used to append arguments to command."""
        cmd = [command_line]
        cmd += [str(arg) for arg in args]
        proc = subprocess.Popen(cmd, stderr=PIPE, stdout=PIPE)
        stdout, stderr = proc.communicate()
        proc.wait()
        if proc.returncode:
            err = CalledProcessError(
                returncode=proc.returncode,
                cmd=" ".join(cmd),
                output=stdout,
            )
            err.stderr = stderr
    return func
