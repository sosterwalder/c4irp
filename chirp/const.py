"""
Constants for chirp.

Config is a special case. Every class that uses Config takes config as an
argument in the constructor, if config is not given it will use the default
config defined here.
"""


def _consts_to_dict(object_):
    """Convert a constants object to a dictionary."""
    new = {}
    for const in dir(object_):
        if not const.startswith("_"):
            new[getattr(object_, const)] = const
    return new


class IPProtocol(object):
    """IP Protocol selection."""

    IPV4 = 2 ** 0
    IPV6 = 2 ** 1

IP_PROTOCOL_DICT = _consts_to_dict(IPProtocol)


class Config(object):
    """Default configuration of chirp."""

    REUSE_TIME    = 30
    """Time till a connection gets garbage collected in this time the
    connection will be reused."""
    TIMEOUT       = 5
    """General network IO related timout"""
    # 4242 // 42 % 42 * 42 * 4.2 // 1
    PORT          = 2998
    """Default chirp port"""
    BIND_V6       = ""
    """Override bind address for IPV6"""
    BIND_V4       = ""
    """Override bind address for IPV4"""
    IP_PROTOCOLS  = IPProtocol.IPV4 | IPProtocol.IPV6
    """Active IP protocols"""
    BACKLOG       = 100
    """Socket listen backlock"""
    RETRIES       = 1
    """How many times chirp retries if a message is not delivered."""
    DEBUG         = False
    """If True add serial numbers to ping, pong and ack messages."""
    MAX_HANDLERS  = 16
    """Maximum handlers running at the same time. If the maximum is reached the
    protocol will block and timeouts will propagate."""
    REQUIRE_ACK   = True
    """Disable this if you want to manage lost messages and timeouts
    yourself."""
    FLOW_CONTROL  = True
    """Flow control will throttle messages if the remote peer is overloaded. If
    flow control is disabled timeouts may occur and even spread through your
    system."""
    RESOLVE       = False
    """Resolving hostnames takes a lot of time and is bad for the performance.
    Chirp caches resolved hostnames, but there is still a performance impact,
    therefore by default we don't resolve hostnames."""
    DISABLE_TASKS = False
    """Send all messages to the default handlers. This is useful if you write
    a message router that isn't interested in the tasks."""
    DISABLE_TLS   = False
    """Do not use TLS encryption. Not recommended."""
    LOCALHOST_OPT = True
    """Optimize localhost connections by disabling TLS"""
    DH_PARAMS_PEM  = None
    """Diffie-hellmann parameters used for perfect forward secrecy.

    .. code-block:: bash

        openssl dhparam -out dh.pem 2048
    """
    CERT_CHAIN_PEM = None
    """Set a certification chain for TLS. If you don't set this a default
    self-sign cert is used, which is not recommended.  Create a certication
    authoritity and sign the certs of your nodes. The pem file should include
    the public- andprivate-key of the certificate and the public-key of the
    certification authority.

    How to create certificates
    --------------------------

    Create the private-key of your ceriticiation authority.

    .. code-block:: bash

        openssl genrsa -out ca-key.pem 4096

    Create the root certificate of your authority.

    .. code-block:: bash

        openssl req -x509 -new -nodes -key ca-key.pem \
        -days 3650 -out ca-root.pem -sha512

    Create the private-key of your certificate.

    .. code-block:: bash

        openssl genrsa -out cert-key.pem 4096

    Create certification request for your certificate.

    .. code-block:: bash

        openssl req -new -key cert-key.pem -out cert.csr -sha512

    Sign your certificate.

    .. code-block:: bash

        openssl x509 -req -in cert.csr -CA ca-root.pem -CAkey ca-key.pem \
        -CAcreateserial -out cert-pub.pem -days 3650 -sha512

    Join the private- and public-keys (certs). ca-key.pem has to stay secret!

    .. code-block:: bash

        cat cert-key.pem > cert.pem
        cat cert-pub.pem >> cert.pem
        cat ca-root.pem >> cert.pem

    Or use c4irp/build/makepki

    .. code-block:: bash

        cd c4irp/build/makepki
        make
        make chain NAME=customerA

    """
