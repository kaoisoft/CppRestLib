#include "RestRequest.h"

#include <StringUtils.h>

using namespace kaoisoft;
using namespace std;

RestRequest::RestRequest()
{
    method = Method::INVALID;
    path = "/";
    protocol = "HTTP1.1";
    body = "";
}

RestRequest::RestRequest(const char* data, size_t len)
{
    // Convert the data to a string
    string str(data, 0, len);

    // Parse the first line
    size_t start = 0;
    size_t index = str.find("\r\n", start);
    string line(data, 0, index);
    shared_ptr<vector<string>> parts = StringUtils::split(line, " ");
    if (3 != parts->size())
    {
        return;
    }
    string methodStr = parts->at(0);
    setMethod(methodStr);
    path = parts->at(1);
    protocol = parts->at(2);

    // Parse out the headers
    start = index + 2;    // skip over the first line's CRLF
    while (string::npos != (index = str.find("\r\n", start)))
    {
        // Get the next line
        line = StringUtils::trim(string(data, start, index - start));
        if (0 == line.length())     // The line after the last header will be blank
        {
            start = index + 2;
            break;
        }

        // Get the header and its value
        parts = StringUtils::split(line, ":");
        if (2 == parts->size())
        {
            headers[StringUtils::trim(parts->at(0))] = StringUtils::trim(parts->at(1));
        }

        // Prepare for the next line
        start = index + 2;
    }

    // Copy the rest of the data into the body
    if (start < len)
    {
        body = string(data, start, len - start);
    }
}

void RestRequest::addHeader(std::string name, std::string value)
{
    headers[name] = value;
}

void RestRequest::getHeaders(std::map<std::string, std::string>* headers)
{
    map<string, string>::iterator iter;
    for (iter = (this->headers).begin(); iter != (this->headers).end(); iter++)
    {
        (*headers)[iter->first] = iter->second;
    }
}

void RestRequest::addParam(std::string name, std::string value)
{
    params[name] = value;
}

string RestRequest::getMethodString(Method method)
{
    switch(method)
    {
    case GET:
        return "GET";

    case POST:
        return "POST";

    case PUT:
        return "PUT";

    case DELETE:
        return "DELETE";

    default:
        return "INVALID";
    }
}

void RestRequest::setHeaders(std::map<std::string, std::string>* headers)
{
    (this->headers).clear();
    map<string, string>::iterator iter;
    for (iter = headers->begin(); iter != headers->end(); iter++)
    {
        (this->headers)[iter->first] = iter->second;
    }
}

void RestRequest::setMethod(string methodStr)
{
    if (0 == methodStr.compare("GET"))
    {
        method = Method::GET;
    }
    else if (0 == methodStr.compare("POST"))
    {
        method = Method::POST;
    }
    else if (0 == methodStr.compare("PUT"))
    {
        method = Method::PUT;
    }
    else if (0 == methodStr.compare("DELETE"))
    {
        method = Method::DELETE;
    }
    else
    {
        method = Method::INVALID;
    }
}

void RestRequest::setParams(std::map<std::string, std::string> params)
{
    (this->params).clear();
    map<string, string>::iterator iter;
    for (iter = params.begin(); iter != params.end(); iter++)
    {
        (this->params)[iter->first] = iter->second;
    }
}

string RestRequest::toString()
{
    string str;

    str.append(getMethodString(method));
    str.append(" ");
    str.append(path);
    str.append(" ");
    str.append(protocol);
    str.append("\r\n");

    map<string, string>::iterator iter;
    for (iter = headers.begin(); iter != headers.end(); iter++)
    {
        str.append(iter->first);
        str.append(": ");
        str.append(iter->second);
        str.append("\r\n");
    }
    str.append("\r\n");

    if (0 < body.length())
    {
        str.append(body);
    }

    return str;
}
