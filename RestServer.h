#ifndef RESTSERVER_H
#define RESTSERVER_H

#include "RestRequest.h"
#include "RestResponse.h"
#include "Socket.h"

#include <log4cxx/logger.h>

namespace kaoisoft
{
    /**
     * Request handler
     *
     * Parameters:
     *   RestRequest* - client's request
     *   void* - extra data passed to the handler, this is set in the addRoute() method
     */
    typedef RestResponse*(*ROUTE_HANDLER)(RestRequest*, void*);

    /**
     * Data associated with a request handler
     */
    struct HandlerData {
        ROUTE_HANDLER handler;      // Handler function pointer
        void* extra;                // Extra data passed to the handler
    };

    /** Forward reference */
    class RestServer;

    /**
     * Arguments passed to the client handler threads.
     */
    struct ClientHandlerArgs
    {
        RestServer* inst;		// Pointer the class instance
        Socket* sock;           // Pointer to client connection
    };

    /**
     * REST server that does not use TLS/SSL to secure the communications.
     */
    class RestServer
    {
    protected:
        int port;                                       // Port that clients will connect to
        Socket* listenSocket;                           // Socket that clients will connect to
        bool listening;                                 // Whether the server is accepting client connections
        pthread_t threadId;                             // ID of the thread that is accepting client connections
        std::map<std::string, HandlerData*> routes;     // Map of paths and their handlers
        std::string missingPageText;                    // HTML response for missing page
        log4cxx::LoggerPtr logger;                      // Logger for instances of this class

    protected:
        static bool initialized;        // Whether the OpenSSL library has been initialized
        static std::string version;     // Library's version

    protected:
        /**
         * Creates the socket that the clients will connect to.
        *
        * @param port Port that the socket will be bound to
        *
        * @return true if successful
         */
        bool createListenSocket(int port);

        /**
         * Creates an instance of the socket wrapper class
         *
         * @param sock Handle to the socket to wrap
         *
         * @return
         */
        virtual Socket* createSocketObject(int sock);

        /**
         * Manages a client connection.
         *
         * @param sock Connection to the client
         */
        virtual void manageClient(Socket* sock);

        /**
         * Routes a request to the appropriate handler.
         *
         * @param request Client's request
         *
         * @return The handler's generated response
         */
        RestResponse* routeRequest(RestRequest* request);

    protected:

        /** Handler for paths that don't have a defined handler */
        static kaoisoft::RestResponse* defaultHandler(kaoisoft::RestRequest* request, void* extra);

        /**
         * Generates a route key for the routing map.
         *
         * @param method
         * @param path
         *
         * @return
         */
        static std::string generateRouteKey(kaoisoft::RestRequest::Method method, std::string path);

        /**
         * Gets the components that were used to generate a route key.
         *
         * @param key Route key string
         * @param method String into which the request method is stored
         * @param path String into which the path is stored
         */
        static void getRouteKeyComponents(std::string key, std::string& method,
                                          std::string& path);

        /** Handler that returns a list of defined routes */
        static kaoisoft::RestResponse* getRoutes(kaoisoft::RestRequest* request, void* extra);

        /** Handler that returns the library's version */
        static kaoisoft::RestResponse* getVersion(kaoisoft::RestRequest* request, void* extra);

        /**
         * Reads the body of a REST request. Reads until there is no data left
         * to read.
         *
         * @param sslSocket Pointer to secure socket wrapper
         *
         * @return The data read
         */
        static std::string readBody(Socket* socket);

        /**
         * Reads a line (until a linefeed is encountered).
         *
         * @param sslSocket Pointer to secure socket wrapper
         *
         * @return The line read, excluding the trailing CR/LF
         */
        static std::string readLine(Socket* socket);

        /**
         * Reads a REST request from a socket.
         */
        static RestRequest* readRequest(Socket* socket);

    public:
        RestServer();
        virtual ~RestServer();

        /**
         * Adds a request route.
         *
         * @param method REST request method
         * @param path Request path
         * @param handler Handler function that will be called
         * @param extraData Pointer to extra data that will be passed to the handler
         */
        void addRoute(kaoisoft::RestRequest::Method method, std::string path,
            ROUTE_HANDLER handler, void* extraData);

        /**
         * Prepares the server to begin handling clients.
         *
        * @param ca_pem Certificate Authorities certificate file
        * @param cert_pem Server's public certificate file
        * @param key_pem Server's private key file
        *
        * @return true if successful
        */
        bool setUp(std::string port_str);

        /**
         * Tells the server to accept and manage client connections.
         */
        void start();

        /**
         * Tells the server to stop accepting client connections.
         */
        void stop();

    public:
        /**
         * Runs in its own thread and listens for and accepts client connections.
         *
         * @param Pointer to class instance
         *
         * @return nullptr
         */
        static void* clientAcceptThread(void *args);

        /**
         * Runs in its own thread and manages a connection to a single client.
         *
         * @param Pointer to a struct contains data needed to manage the connection
         *
         * @return nullptr
         */
        static void* clientHandlerThread(void *args);
    };
}

#endif // RESTSERVER_H
