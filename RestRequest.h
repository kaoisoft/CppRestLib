#ifndef RESTREQUEST_H
#define RESTREQUEST_H

#include <map>
#include <string>
#include <vector>

namespace kaoisoft {
    /**
     * REST request
     *
     * Example:
     *      GET / HTTP/1.1
     *      Host: localhost:4433
     *      User-Agent: curl/7.68.0
     *      Accept: application/json
     *      <blank line>
     *      <body, if present>
     */
    class RestRequest
    {
    public:
        enum Method { GET, POST, PUT, DELETE, INVALID };

    private:
        Method method;                                  // Request method
        std::string path;                               // Request path (i.e.: /test)
        std::map<std::string, std::string> params;      // Map of request parameters
        std::string protocol;                           // HTTP protocol (HTTP/?.?)
        std::map<std::string, std::string> headers;     // headers
        std::string body;                               // Body text

    public:
        RestRequest();
        RestRequest(const char* data, size_t len);

        std::string getBody() { return body; }
        void setBody(std::string body) { this->body = body; }

        void getHeaders(std::map<std::string, std::string>* headers);
        void addHeader(std::string name, std::string value);
        void setHeaders(std::map<std::string, std::string>* headers);

        Method getMethod() { return method; }
        void setMethod(Method method) { this->method = method; }
        void setMethod(std::string methodStr);

        std::string getPath() { return path; }
        void setPath(std::string path) { this->path = path; }

        std::map<std::string, std::string> getParams() { return params; }
        void addParam(std::string name, std::string value);
        void setParams(std::map<std::string, std::string> params);

        std::string getProtocol() { return protocol; }
        void setProtocol(std::string protocol) { this->protocol = protocol; }

        std::string toString();

    public:
        /**
         * Gets a textual representation of a Method.
         *
         * @param method
         * @return
         */
        static std::string getMethodString(Method method);
    };
};

#endif // RESTREQUEST_H
