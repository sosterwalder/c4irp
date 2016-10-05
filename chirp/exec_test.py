"""Running test executables."""
# Start ignoring PyLintBear*E1101*

from . import sh

ssl_client = sh.command("src/basic_ssl_client_etest")
ssl_server = sh.background_command("src/basic_ssl_server_etest")

# Stop ignoring
