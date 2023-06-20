#include "Socket.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace log4cxx;
using namespace kaoisoft;

Socket::Socket()
{
    sock = -1;
    remoteAddr = "undefined";

    // Create the logger
    logger = Logger::getLogger("com.kaoisoft.Socket");
}

Socket::Socket(int sock)
{
    this->sock = sock;
    remoteAddr = "undefined";
}

Socket::~Socket()
{
    // Close the socket
    if (-1 != sock)
    {
        close(sock);
    }
}

int Socket::read(char* buff, int max)
{
    int ret = ::read(sock, buff, max);
    if (-1 == ret)
    {
        if (EWOULDBLOCK == errno || EAGAIN == errno)
        {
            return 0;
        }
    }
    return ret;
}

bool Socket::setNonBlocking()
{
    // Get the socket's current flags
    int flags = fcntl(sock, F_GETFL);
    if (-1 == flags)
    {
        LOG4CXX_ERROR(logger, "Cannot set socket to non-blocking. Could not get socket's flags: " << strerror(errno));
        return false;
    }

    // Set the non-blocking flag
    if (-1 == fcntl(sock, F_SETFL, flags | O_NONBLOCK))
    {
        LOG4CXX_ERROR(logger, "Cannot set socket to non-blocking: " << strerror(errno));
        return false;
    }

    return true;
}

int Socket::write(const char* buff, int len)
{
    int ret = ::write(sock, buff, len);
    if (-1 == ret)
    {
        if (EWOULDBLOCK == errno || EAGAIN == errno)
        {
            return 0;
        }
    }
    return ret;
}
