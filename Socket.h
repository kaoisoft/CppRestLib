#ifndef SOCKET_H
#define SOCKET_H

#include <log4cxx/logger.h>

#include <string>

namespace kaoisoft
{
    /**
     * Wrapper around a POSIX TCP socket.
     */
    class Socket
    {
    private:
        int sock;                   // Socket handle
        std::string remoteAddr;     // Address of the remote end of the connection

    protected:
        log4cxx::LoggerPtr logger;  // Logging object

    public:
        Socket();
        Socket(int sock);
        virtual ~Socket();

        /**
         * Gets the wrapped socket handle.
         *
         * @return
         */
        int getHandle() { return sock; }

        std::string getRemoteAddress() { return remoteAddr; }

        /**
         * Reads data from the socket.
         *
         * @param buff Buffer into which the data is stored
         * @param max Maximum number of bytes to read
         *
         * @return The actual number of bytes read or -1 if an error occurred
         */
        virtual int read(char* buff, int max);

        /**
         * Sets the socket to non-blocking mode.
         *
         * @return true if successful
         */
        bool setNonBlocking();

        void setRemoteAddress(std::string remoteAddr) { this->remoteAddr = remoteAddr; }

        /**
         * Writes data to the socket.
         *
         * @param buff Buffer containing the data to write
         * @param len Number of bytes to write
         *
         * @return The actual number of bytes written or -1 if an error occurred
         */
        virtual int write(const char* buff, int len);
    };
}

#endif // SOCKET_H
