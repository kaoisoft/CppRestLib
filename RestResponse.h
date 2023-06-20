#ifndef RESTRESPONSE_H
#define RESTRESPONSE_H

#include <map>
#include <string>

namespace kaoisoft
{
    /**
     * Data sent back to a client in response to a request.
     */
    class RestResponse
    {
    private:
        std::string protocol;
        int code;
        std::string reason;
        std::map<std::string, std::string> headers;
        std::string body;

    public:
        RestResponse();
        virtual ~RestResponse();

        std::string getBody() { return body; }
        void setBody(std::string body) { this->body = body; }

        int getCode() { return code; }
        void setCode(int code) { this->code = code; }

        void addHeader(std::string name, std::string value);
        void getHeaders(std::map<std::string, std::string>* headers);
        void setHeaders(std::map<std::string, std::string> headers);

        std::string getProtocol() { return protocol; }
        void setProtocol(std::string protocol) { this->protocol = protocol; }

        std::string getReason() { return reason; }
        void setReason(std::string reason) { this->reason = reason; }

        std::string toString();
    };
};

#endif // RESTRESPONSE_H
