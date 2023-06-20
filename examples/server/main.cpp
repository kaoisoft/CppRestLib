/**
 * The system on which this sample was built has the CppRestLib include files
 * stored in a directory named 'CppRestLib' (see the Makefile).
 * 
 * Also, in order to run both the server example and the curl client, make sure
 * to set the certificate paths correctly.
 * 
 * To test this example, once it is running, issue following curl commands:
 * 
 * curl -X GET https://<host>:4433/hello \
 *  --cacert /home/user/certs/two-way/ca.crt \
 *  --cert /home/user/certs/two-way/client.crt \
 *  --key /home/user/certs/two-way/client_rsa_private.pem \
 *  --pass password \
 *  --http0.9 \
 *  -v \
 *  -H "Content-Type=application/json"
 *
 * curl -X GET http://<host>:8080/hello --http0.9 -v -H "Content-Type=application/json"
 */

#include <CppRestLib/RestServer.h>
#include <CppRestLib/SecureRestServer.h>

// CppRestLib uses log4cxx internally, so we must initialize it
#include <log4cxx/xml/domconfigurator.h>

#include <unistd.h>

#include <map>
#include <string>
#include <vector>

using namespace kaoisoft;
using namespace std;

// The paths to the certificate files
string CERTIFICATE_FILE = "/home/user/certs/two-way/server.crt";
string PRIVATE_KEY_FILE = "/home/user/certs/two-way/server_rsa_private.pem";
string CA_CERTIFICATE = "/home/user/certs/two-way/ca.crt";

/**
 * Sample REST endpoint for the non-secure server.
 *
 * @param request Client's REST request
 * @param extra
 *
 * @return
 */
static RestResponse* nonSecureHello(RestRequest* request, void* extra)
{
	// Create the response headers. A copy of these headers will be stored in
	// the response, so we can just pass in the address of a local variable.
	map<string, string> headers;
	headers["Content-Type"] = "application/json";
	
	// Build the response. The server instance will delete the response.
	RestResponse* response = new RestResponse;
	response->setCode(200);			// Set the response code
	response->setReason("OK");		// Set the response reason
	response->setHeaders(headers);	// Set the response headers
	response->setBody("{\"data\":\"Non-secure Hello!\"}");
	
	return response;
}

/**
 * Sample REST endpoint for the secure server.
 *
 * @param request Client's REST request
 * @param extra
 *
 * @return
 */
static RestResponse* secureHello(RestRequest* request, void* extra)
{
	// Create the response headers. A copy of these headers will be stored in
	// the response, so we can just pass in the address of a local variable.
	map<string, string> headers;
	headers["Content-Type"] = "application/json";
	
	// Build the response. The server instance will delete the response.
	RestResponse* response = new RestResponse;
	response->setCode(200);			// Set the response code
	response->setReason("OK");		// Set the response reason
	response->setHeaders(headers);	// Set the response headers
	response->setBody("{\"data\":\"Secure Hello!\"}");
	
	return response;
}

/**
 * Example REST server using CppRestLib.
 */
int main()
{
	// CppRestLib uses log4cxx internally, so we must initialize it
    log4cxx::xml::DOMConfigurator::configure("./log4cxx_config.xml");

    // Create a non-secure REST server
    RestServer restServer;
	restServer.setUp("8080");

    // Create a secure REST server
    SecureRestServer secureServer;
	secureServer.setUp("4433", CA_CERTIFICATE, CERTIFICATE_FILE,
                                  PRIVATE_KEY_FILE);

    // Define the routes and their handlers
	//
	// The fourth argument can be used to pass in a pointer to a class instance, 
	// such as a database access object, that might be needed to build the response.
    restServer.addRoute(RestRequest::GET, "/hello", nonSecureHello, (void*)nullptr);
    secureServer.addRoute(RestRequest::GET, "/hello", secureHello, (void*)nullptr);

    // Start the servers. These will run in their own threads.
	restServer.start();
    secureServer.start();

    // Run forever so that the servers can run in their own threads
    while(1) {
        sleep(1);
    }
}
