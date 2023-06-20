#ifndef SECURERESTSERVER_H
#define SECURERESTSERVER_H

#include "RestServer.h"
#include "SslSocket.h"
#include "RestRequest.h"
#include "RestResponse.h"

#include <openssl/ssl.h>
#include <pthread.h>

#include <string>

namespace kaoisoft
{
	/**
	* REST server that communicates with clients via secure communications.
	*/
    class SecureRestServer : public RestServer
	{
	private:
        SSL_CTX* ctx;                   // Context for secure communications with clients

    private:
        static bool initialized;        // Whether the OpenSSL library has been initialized

	private:

        /**
		* Creates the secure context for communications.
		* 
		* @param ca_pem Certificate Authorities certificate file
		* @param cert_pem Server's public certificate file
		* @param key_pem Server's private key file
		* 
		* @return true if successful
		*/
		bool createSecureContext(std::string ca_pem,
								std::string cert_pem,
								std::string key_pem);

    protected:

        /**
         * Creates an instance of the socket wrapper class
         *
         * @param sock Handle to the socket to wrap
         *
         * @return
         */
        virtual Socket* createSocketObject(int sock) override;

        /**
         * Manages a client connection.
         *
         * @param sock Connection to the client
         */
        virtual void manageClient(Socket* sock) override;

	public:
		SecureRestServer();
		~SecureRestServer();

		/**
		 * Prepares the server to begin handling clients.
		 * 
		* @param ca_pem Certificate Authorities certificate file
		* @param cert_pem Server's public certificate file
		* @param key_pem Server's private key file
		* 
		* @return true if successful
		*/
		bool setUp(
			std::string port_str,
			std::string ca_pem,
			std::string cert_pem,
			std::string key_pem);
	};
}

#endif  // SECURERESTSERVER_H
