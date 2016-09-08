// I did not make this terrible code, but I also don't care fixing it since it
// is only a test.
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define FAIL    -1

int OpenListener(int port) {
    int sd;
    int set = 1;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port"); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port"); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
    return sd;
}

SSL_CTX* InitServerCTX(void) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf( // NOCOV
            stderr, // NOCOV
            "Private key does not match the public certificate\n" // NOCOV
        ); // NOCOV I do not care
        abort(); // NOCOV I do not care
    }
}

void ShowCerts(SSL* ssl) {
    X509 *cert;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        char *line; // NOCOV I do not care
        printf("Server certificates:\n"); // NOCOV I do not care
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0); // NOCOV
        printf("Subject: %s\n", line); // NOCOV I do not care
        free(line); // NOCOV I do not care
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0); // NOCOV
        printf("Issuer: %s\n", line); // NOCOV I do not care
        free(line); // NOCOV I do not care
        X509_free(cert); // NOCOV I do not care
    } // NOCOV I do not care
    else
        printf("No certificates.\n"); // NOCOV I do not care
}

void Servlet(SSL* ssl) /* Serve the connection -- threadable */ {
    int sd;

    if ( SSL_accept(ssl) == FAIL )     /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr); // NOCOV I do not care
    else
    {
        int bytes;
        char buf[1024];
        ShowCerts(ssl);        /* get any certificates */
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
        if ( bytes > 0 )
        {
            const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";
            char reply[1024];
            buf[bytes] = 0;
            printf("Client msg: \"%s\"\n", buf);
            sprintf(reply, HTMLecho, buf);   /* construct reply */
            SSL_write(ssl, reply, strlen(reply)); /* send reply */
        }
        else
            ERR_print_errors_fp(stderr); // NOCOV I do not care
    }
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
}

int main(int count, char *strings[]) {
    SSL_CTX *ctx;
    int server;
    char *portnum;

    if ( count != 2 )
    {
        printf("Usage: %s <portnum>\n", strings[0]); // NOCOV I do not care
        exit(0); // NOCOV I do not care
    }
    SSL_library_init();

    portnum = strings[1];
    ctx = InitServerCTX();        /* initialize SSL */
    LoadCertificates(ctx, "chirp/cert.pem", "chirp/cert.pem"); /* load certs */
    server = OpenListener(atoi(portnum));    /* create server socket */
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    SSL *ssl;

    int client = accept(
        server, (struct sockaddr*)&addr, &len
    );  /* accept connection as usual */
    printf("Connection: %s:%d\n",inet_ntoa(
        addr.sin_addr
    ), ntohs(addr.sin_port));
    ssl = SSL_new(ctx);              /* get new SSL state with context */
    SSL_set_fd(ssl, client);      /* set connection socket to SSL state */
    Servlet(ssl);         /* service connection */
    close(server);          /* close server socket */
    SSL_CTX_free(ctx);         /* release context */
}
