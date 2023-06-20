#include "RestServer.h"
#include "StringUtils.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

using namespace kaoisoft;
using namespace std;
using namespace log4cxx;

bool RestServer::initialized = false;
string RestServer::version = "1.0";

RestServer::RestServer()
{
    listenSocket = nullptr;
    listening = false;
    missingPageText = "<html><body><h1>Page Not Found</h1></body></html>";
    port = -1;

    // Create the logger
    logger = Logger::getLogger("com.kaoisoft.RestServer");

    // Add the system routes
    addRoute(RestRequest::Method::GET, "/system/routes",
             RestServer::getRoutes, this);
    addRoute(RestRequest::Method::GET, "/system/version",
             RestServer::getVersion, nullptr);

}

RestServer::~RestServer()
{
    if (nullptr != listenSocket)
    {
        delete listenSocket;
    }

    // Free the routes' resources
    map<string, HandlerData*>::iterator iter;
    for (iter = routes.begin(); iter != routes.end(); iter++)
    {
        // Since the extra pointer is often used to store the class instance,
        // we do not want to delete it.
        delete iter->second;
    }

    LOG4CXX_DEBUG(logger, "Server listening on port " << port << " has been stopped");
}

void RestServer::addRoute(RestRequest::Method method, string path,
                                ROUTE_HANDLER handler, void* extra)
{
    // Create the handler data
    struct HandlerData* handlerData = new struct HandlerData;
    handlerData->handler = handler;
    handlerData->extra = extra;

    // Add the route
    string key = generateRouteKey(method, path);
    routes[key] = handlerData;

    LOG4CXX_DEBUG(logger, "Added route " <<
                  RestRequest::getMethodString(method) << " " << path);
}

void* RestServer::clientAcceptThread(void* args)
{
    struct sockaddr_in sin;
    socklen_t sin_len;
    int sock;

    // Get the class instance
    RestServer* inst = (RestServer*)args;

    // Run until told to stop
    while (inst->listening)
    {
        // Check for a client connection
        sin_len = sizeof(sin);
        sock = accept((inst->listenSocket)->getHandle(), (struct sockaddr *) &sin, &sin_len);
        if (-1 == sock)
        {
            if (EWOULDBLOCK != errno && EAGAIN != errno)
            {
                LOG4CXX_ERROR(inst->logger, "Failed to accept client connection");
            }

            // Pause before checking again
            if (inst->listening)
            {
                sleep(1);
            }
            continue;
        }
        if (!(inst->listening))
        {
            break;
        }

        // Create the socket object
        Socket* socket = inst->createSocketObject(sock);
        socket->setRemoteAddress(inet_ntoa(sin.sin_addr));
        LOG4CXX_DEBUG(inst->logger, "Accepted a connection from a client at " <<
                      socket->getRemoteAddress());

        // Start a thread to manage this client
        struct ClientHandlerArgs* args = new struct ClientHandlerArgs;
        args->inst = inst;
        args->sock = socket;
        pthread_t threadId;
        pthread_create(&threadId, nullptr, RestServer::clientHandlerThread,
                       (void*)args);
    }

    return nullptr;
}

void* RestServer::clientHandlerThread(void *args)
{
    // Save the arguments
    struct ClientHandlerArgs* clientHandlerArgs = (struct ClientHandlerArgs*)args;
    RestServer* inst = clientHandlerArgs->inst;
    Socket* socket = clientHandlerArgs->sock;
    delete clientHandlerArgs;

    inst->manageClient(socket);

    return nullptr;
}

bool RestServer::createListenSocket(int port)
{
    struct sockaddr_in sin;
    int val;

    /* Create a socket */
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG4CXX_ERROR(logger, "Cannot create a socket");
        return false;
    }
    listenSocket = new Socket(sock);

    /* We don't want bind() to fail with EBUSY */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        LOG4CXX_ERROR(logger, "Could not set SO_REUSEADDR on the socket");
        return false;
    }

    // Set the socket to non-blocking
    listenSocket->setNonBlocking();

    /* Fill up the server's socket structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    /* Bind the socket to the specified port number */
    if (bind(listenSocket->getHandle(), (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        LOG4CXX_ERROR(logger, "Could not bind the socket");
        return false;
    }

    /* Specify that this is a listener socket */
    if (listen(listenSocket->getHandle(), SOMAXCONN) < 0) {
        LOG4CXX_ERROR(logger, "Failed to listen on this socket");
        return false;
    }

    return true;
}

Socket* RestServer::createSocketObject(int sock)
{
    return new Socket(sock);
}

void RestServer::manageClient(Socket* sock)
{
    // Set the client socket to non-blocking
    sock->setNonBlocking();

    // Read the request
    RestRequest* request = readRequest(sock);
    LOG4CXX_TRACE(logger, "Received request " << request->toString() <<
                  " from client at " << sock->getRemoteAddress());

    // Route the request to the appropriate handler
    RestResponse* response = routeRequest(request);
    string responseStr = response->toString();

    // Write the response
    sock->write(responseStr.c_str(), responseStr.length());
    LOG4CXX_TRACE(logger, "Sent response " << responseStr <<
                  " to client at " << sock->getRemoteAddress());

    // Clean up
    delete request;
    delete response;
}

RestResponse* RestServer::defaultHandler(RestRequest* request, void* extra)
{
    // Prevent unused parameter warning
    (void)request;

    // Get the server instance
    RestServer* inst = (RestServer*)extra;

    // Send the response
    RestResponse* response = new RestResponse;
    response->setCode(404);
    response->setReason("Not Found");
    map<string, string> headers;
    headers["Content-Type"] = "text/html";
    response->setHeaders(headers);
    response->setBody(inst->missingPageText);

    return response;
}

string RestServer::generateRouteKey(RestRequest::Method method, string path)
{
    return RestRequest::getMethodString(method) + ":" + path;
}

void RestServer::getRouteKeyComponents(string key, string& method,
                                             string& path)
{
    // Split the key on the colon
    shared_ptr<vector<string>> parts = StringUtils::split(key, ":");

    // Save the components
    method = parts->at(0);
    path = parts->at(1);
}

RestResponse* RestServer::getRoutes(RestRequest* request, void* extra)
{
    // Prevent unused parameter warning
    (void)request;

    // Get the server instance
    RestServer* inst = (RestServer*)extra;

    // Build the list of routes
    string str("{\"routes\":[");
    string method, path;
    map<string, HandlerData*>::iterator iter;
    for (iter = (inst->routes).begin(); iter != (inst->routes).end(); iter++)
    {
        // Add a comma after the previous route
        if (iter != (inst->routes).begin())
        {
            str.append(",");
        }

        // Get this route's components
        getRouteKeyComponents(iter->first, method, path);

        // Add the JSON for this route
        str.append("{\"method\":\"");
        str.append(method);
        str.append("\",\"path\":\"");
        str.append(path);
        str.append("\"}");
    }
    str.append("]");

    // Send the response
    RestResponse* response = new RestResponse;
    response->setCode(200);
    response->setReason("OK");
    map<string, string> headers;
    headers["Content-Type"] = "application/json";
    response->setHeaders(headers);
    response->setBody(str);

    return response;
}

RestResponse* RestServer::getVersion(RestRequest* request, void* extra)
{
    // Prevent unused parameter warning
    (void)request;
    (void)extra;

    string body = "{\"version\":\"";
    body.append(version);
    body.append("\"}");

    // Send the response
    RestResponse* response = new RestResponse;
    response->setCode(200);
    response->setReason("OK");
    map<string, string> headers;
    headers["Content-Type"] = "application/json";
    response->setHeaders(headers);
    response->setBody(body);

    return response;
}

string RestServer::readBody(Socket* socket)
{
    string str;

    // Read one character at a time until there is no more data to read.
    char ch;
    int ret;
    while (1 == (ret = socket->read(&ch, 1)))
    {
        str.append(1, ch);
    }

    return str;
}

string RestServer::readLine(Socket* socket)
{
    string str;

    // Read one character at a time until a linefeed is encountered or until
    // there is no more data to read.
    char ch, prev = ' ';
    int ret;
    while (1 == (ret = socket->read(&ch, 1)))
    {
        if ('\n' == ch)
        {
            if ('\r' == prev)
            {
                // Remove the CR from the end of the string
                str = str.substr(0, str.length() - 1);
            }
            break;
        }
        str.append(1, ch);
        prev = ch;
    }

    return str;
}

RestRequest* RestServer::readRequest(Socket* socket)
{
    RestRequest* request = new RestRequest;
    request->setMethod(RestRequest::Method::INVALID);

    // Read the request line
    string line = readLine(socket);
    shared_ptr<vector<string>> parts = StringUtils::split(line, " ");
    if (3 != parts->size())
    {
        printf("Request line '%s' is invalid\n", line.c_str());
        return request;
    }
    request->setMethod(parts->at(0));
    request->setPath(parts->at(1));
    request->setProtocol(parts->at(2));

    // Read the headers
    line = readLine(socket);
    while (0 < line.length())
    {
        parts = StringUtils::split(line, ":");
        request->addHeader(StringUtils::trim(parts->at(0)),
                           StringUtils::trim(parts->at(1)));
        line = readLine(socket);
    }

    // Read the body
    request->setBody(readBody(socket));

    return request;
}

RestResponse* RestServer::routeRequest(RestRequest* request)
{
    // Generate the key for the route
    string routeKey = generateRouteKey(request->getMethod(), request->getPath());

    // Route the request to the associated handler
    HandlerData* handlerData = nullptr;
    map<string, HandlerData*>::iterator iter;
    for (iter = routes.begin(); iter != routes.end(); iter++)
    {
        if (0 == (iter->first).compare(routeKey))
        {
            handlerData = iter->second;
            break;
        }
    }
    if (nullptr == handlerData)
    {
        handlerData = new HandlerData;
        handlerData->handler = RestServer::defaultHandler;
        handlerData->extra = this;
    }

    // Call the handler
    RestResponse* response = (handlerData->handler)(request, handlerData->extra);

    return response;
}

bool RestServer::setUp(string port_str)
{
    // save the port
    port = std::stoi(port_str);

    if (!createListenSocket(port))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void RestServer::start()
{
    // Set the listening flag
    listening = true;

    // Start the thread that will accept client connections
    pthread_create(&threadId, nullptr, RestServer::clientAcceptThread,
                   (void*)this);
}

void RestServer::stop()
{
    listening = false;

    // Kill the thread
    pthread_cancel(threadId);
}
