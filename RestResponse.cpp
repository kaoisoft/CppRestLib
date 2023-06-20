#include "RestResponse.h"

using namespace kaoisoft;
using namespace std;

RestResponse::RestResponse()
{
    protocol = "HTTP/1.1";
    code = 500;
    reason = "Internal Server Error";
}

RestResponse::~RestResponse()
{
}

void RestResponse::addHeader(string name, string value)
{
    headers[name] = value;
}

void RestResponse::getHeaders(std::map<std::string, std::string>* headers)
{
    map<string, string>::iterator iter;
    for (iter = (this->headers).begin(); iter != (this->headers).end(); iter++)
    {
        (*headers)[iter->first] = iter->second;
    }
}

void RestResponse::setHeaders(map<string, string> headers)
{
    // Remove the existing headers
    (this->headers).clear();

    // Copy the supplied headers
    map<string, string>::iterator iter;
    for (iter = headers.begin(); iter != headers.end(); iter++)
    {
        (this->headers)[iter->first] = iter->second;
    }
}

/**
 * Builds the response into a string to send to the client.
 *
 * @return
 */
string RestResponse::toString()
{
    string str = "";

    str.append(protocol);
    str.append(" ");
    str.append(std::to_string(code));
    str.append(" ");
    str.append(reason);
    str.append("\r\n");
    if (0 < headers.size())
    {
        map<string, string>::iterator iter;
        for (iter = headers.begin(); iter != headers.end(); iter++)
        {
            str.append(iter->first);
            str.append(": ");
            str.append(iter->second);
            str.append("\r\n");
        }
    }
    str.append("Content-Length: ");
    str.append(std::to_string(body.length()));
    str.append("\r\n");
    str.append("\r\n");
    if (0 < body.length())
    {
        str.append(body);
    }

    return str;
}
