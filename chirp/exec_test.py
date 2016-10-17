"""Running test executables."""
# Start ignoring PyLintBear*E1101*

import os
import signal
import socket
import ssl
import time

from . import sh

chirp = sh.background_command("src/chirp_etest")


def connect_ssl():
    """Do an ssl handshake."""
    context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    context.verify_mode = ssl.CERT_REQUIRED
    context.check_hostname = False
    folder = __file__.split(os.path.sep)[:-1]
    folder.append("cert.pem")
    cert = "%s%s" % (
        os.path.sep,
        os.path.join(*folder)
    )
    context.load_verify_locations(cert)
    context.load_cert_chain(cert)
#    context.set_ciphers(
#        "-ALL:"
#        "DHE-DSS-AES256-GCM-SHA384:"
#        "DHE-RSA-AES256-GCM-SHA384:"
#        "DHE-RSA-AES256-SHA256:"
#        "DHE-DSS-AES256-SHA256:"
#    )

    conn = context.wrap_socket(socket.socket(socket.AF_INET))
    conn.connect(('localhost', 2998))
    conn.sendall(b'huhu')


def test_ssl():
    """Test if chirp binary can do an openssl handshake."""
    p = chirp()
    time.sleep(0.25)
    try:
        connect_ssl()
    finally:
        try:
            time.sleep(0.25)
            p.send_signal(signal.SIGINT)
            stdout, stderr = p.communicate()
            print(stdout.decode("UTF-8"), stderr.decode("UTF-8"))
        finally:
            p.kill()

if __name__ == "__main__":
    connect_ssl()


# Stop ignoring
