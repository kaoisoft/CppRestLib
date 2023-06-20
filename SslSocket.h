#ifndef _SSLSOCKET_H
#define _SSLSOCKET_H

#include "Socket.h"

#include <openssl/ssl.h>

#include <string>

namespace kaoisoft
{
	/**
	* Wrapper class for an TSL-secured socket.
	*/
    class SslSocket : public Socket
	{
	private:
        SSL* ssl;       // Secured connection
		
	public:
        SslSocket(SSL_CTX* ctx, int sock);
        SslSocket(SSL* ssl);
		virtual ~SslSocket();
		
        /**
         * May be called after performHandshake() to verify that the client's
         * presented certificate was accepted.
         *
         * @return true is the client's certificate was accepted
         */
        bool clientVerified();

        /**
         * Retrieves the clients Distinguished Name from its certificate.
         *
         * The DN can be used to control access to resources, such as end-points.
         *
         * @return
         */
        std::string getClientDN();

		/**
		 * Performs the SSL handshake with the client.
		 * 
		 * @return 1 if successful
		 */
		int performHandshake();
		
		/**
		 * Reads data from the socket.
		 * 
		 * @param buff Buffer into which the data is stored
		 * @param max Maximum number of bytes to read
		 * 
		 * @return The actual number of bytes read or -1 if an error occurred
		 */
        virtual int read(char* buff, int max) override;
		
        void setSsl(SSL* ssl) { this->ssl = ssl; }

		/**
		 * Writes data to the socket.
		 * 
		 * @param buff Buffer containing the data to write
		 * @param len Number of bytes to write
		 * 
		 * @return The actual number of bytes written or -1 if an error occurred
		 */
        virtual int write(const char* buff, int len) override;
	};
}

#endif	// _SSLSOCKET_H
