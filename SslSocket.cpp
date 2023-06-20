#include "SslSocket.h"

#include <openssl/x509.h>

using namespace kaoisoft;
using namespace std;
using namespace log4cxx;

SslSocket::SslSocket(SSL_CTX* ctx, int sock) : Socket(sock)
{
    // Create the logger
    logger = Logger::getLogger("com.kaoisoft.SslSocket");

    /* Get an SSL handle from the context */
    if (!(ssl = SSL_new(ctx))) {
        fprintf(stderr, "Could not get an SSL handle from the context\n");
        ssl = nullptr;
        return;
    }

    /* Associate the newly accepted connection with this handle */
    SSL_set_fd(ssl, sock);
}

SslSocket::SslSocket(SSL* ssl) : Socket(SSL_get_fd(ssl))
{
    this->ssl = ssl;
}

SslSocket::~SslSocket()
{
    if (nullptr != ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
}

bool SslSocket::clientVerified()
{
    return X509_V_OK == SSL_get_verify_result(ssl);
}

string SslSocket::getClientDN()
{
    // Get the certificate that the client presented
    X509* x509 = SSL_get_peer_certificate(ssl);
    if (nullptr == x509)
    {
        return "";
    }

    string subject = X509_NAME_oneline(X509_get_subject_name(x509), nullptr, 0);

    // Free the certificate reference
    X509_free(x509);

    return subject;
}

int SslSocket::performHandshake()
{
	return SSL_accept(ssl);
}

int SslSocket::read(char* buff, int max)
{
    int ret = SSL_read(ssl, buff, max);
    if (0 >= ret)
    {
        int err = SSL_get_error(ssl, ret);
        if (SSL_ERROR_WANT_READ == err)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return ret;
    }
}

int SslSocket::write(const char* buff, int len)
{
    int ret = SSL_write(ssl, buff, len);
    if (0 >= ret)
    {
        int err = SSL_get_error(ssl, ret);
        if (SSL_ERROR_WANT_WRITE == err)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return ret;
    }
}
