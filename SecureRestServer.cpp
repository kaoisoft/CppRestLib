#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#include "StringUtils.h"

#include "SecureRestServer.h"
#include "SslSocket.h"

using namespace kaoisoft;
using namespace std;
using namespace log4cxx;

/* Buffer size to be used for transfers */
#define BUFSIZE 128

bool SecureRestServer::initialized = false;

SecureRestServer::SecureRestServer()
{
    ctx = nullptr;

	// Initialize OpenSSL
	if (!initialized)
	{
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		initialized = true;
	}

    // Create the logger
    logger = Logger::getLogger("com.kaoisoft.SecureRestServer");
}

SecureRestServer::~SecureRestServer()
{
	if (nullptr != ctx)
	{
		SSL_CTX_free(ctx);
	}
}

bool SecureRestServer::createSecureContext(string ca_pem,
										   string cert_pem,
										   string key_pem)
{
    /* Get a default context */
    if (!(ctx = SSL_CTX_new(SSLv23_server_method()))) {
        LOG4CXX_ERROR(logger, "SSL_CTX_new failed");
        return false;
    }

    /* Set the CA file location for the server */
    if (SSL_CTX_load_verify_locations(ctx, ca_pem.c_str(), NULL) != 1) {
        LOG4CXX_ERROR(logger, "Could not set the CA file location");
		SSL_CTX_free(ctx);
		ctx = nullptr;
        return false;
    }

    /* Load the client's CA file location as well */
    SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(ca_pem.c_str()));

    /* Set the server's certificate signed by the CA */
    if (SSL_CTX_use_certificate_file(ctx, cert_pem.c_str(), SSL_FILETYPE_PEM) != 1) {
        LOG4CXX_ERROR(logger, "Could not set the server's certificate");
		SSL_CTX_free(ctx);
		ctx = nullptr;
        return false;
    }

    /* Set the server's key for the above certificate */
    if (SSL_CTX_use_PrivateKey_file(ctx, key_pem.c_str(), SSL_FILETYPE_PEM) != 1) {
        LOG4CXX_ERROR(logger, "Could not set the server's key");
		SSL_CTX_free(ctx);
		ctx = nullptr;
        return false;
    }

    /* We've loaded both certificate and the key, check if they match */
    if (SSL_CTX_check_private_key(ctx) != 1) {
        LOG4CXX_ERROR(logger, "Server's certificate and the key don't match");
		SSL_CTX_free(ctx);
		ctx = nullptr;
        return false;
    }

    /* We won't handle incomplete read/writes due to renegotiation */
    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    /* Specify that we need to verify the client as well */
    SSL_CTX_set_verify(ctx,
                       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       NULL);

    /* We accept only certificates signed only by the CA himself */
    SSL_CTX_set_verify_depth(ctx, 1);

    return true;
}

Socket* SecureRestServer::createSocketObject(int sock)
{
    return new SslSocket(ctx, sock);
}

void SecureRestServer::manageClient(Socket* sock)
{
    // Cast the connection object to a secure object
    SslSocket* sslSocket = (SslSocket*)sock;

    // Perform secure handshake with the client
    if (sslSocket->performHandshake() != 1) {
        LOG4CXX_ERROR(logger, "Could not perform SSL handshake");
        return;
    }
    if (!sslSocket->clientVerified())
    {
        LOG4CXX_ERROR(logger, "Client's certifcate is not acceptable");
        return;
    }
    LOG4CXX_DEBUG(logger, "Accepted client certificate with DN " <<
                  sslSocket->getClientDN());

    // Set the client socket to non-blocking
    sslSocket->setNonBlocking();

    // Read the request
    RestRequest* request = readRequest(sslSocket);
    LOG4CXX_TRACE(logger, "Received request " << request->toString() <<
                  " from client at " << sock->getRemoteAddress());

    // Route the request to the appropriate handler
    RestResponse* response = routeRequest(request);
    string responseStr = response->toString();

    // Write the response
    sslSocket->write(responseStr.c_str(), responseStr.length());
    LOG4CXX_TRACE(logger, "Sent response " << responseStr <<
                  " to client at " << sock->getRemoteAddress());

    // Clean up
    delete request;
    delete response;
}

bool SecureRestServer::setUp(
			std::string port_str,
			std::string ca_pem,
			std::string cert_pem,
			std::string key_pem)
{
    if (!RestServer::setUp(port_str))
	{
		return false;
	}
	
	return createSecureContext(ca_pem, cert_pem, key_pem);
}
