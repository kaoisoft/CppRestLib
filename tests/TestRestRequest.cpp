#include <RestRequest.h>

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <list>
#include <map>

using namespace CppUnit;
using namespace std;
using namespace kaoisoft;

//-----------------------------------------------------------------------------

class TestRestRequest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestRestRequest);
    CPPUNIT_TEST(testConstructor);
//    CPPUNIT_TEST(testMultiply);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testConstructor(void);

private:

    RestRequest* request;
};

//-----------------------------------------------------------------------------

void
TestRestRequest::testConstructor(void)
{
    string path = "/";
    string protocol = "HTTP/1.1";
    string headerName1 = "Header1";
    string headerValue1 = "header one";
    string headerName2 = "Header2";
    string headerValue2 = "header two";
    string body = "This is the body's text";

    // Build the request text
    string str;
    str.append("GET ");
    str.append(path);
    str.append(" ");
    str.append(protocol);
    str.append("\r\n");
    str.append(headerName1);
    str.append(": ");
    str.append(headerValue1);
    str.append("\r\n");
    str.append(headerName2);
    str.append(": ");
    str.append(headerValue2);
    str.append("\r\n");
    str.append("\r\n");
    str.append(body);

    // Create the instance
    RestRequest request(str.c_str(), str.length());

    // Verify that the fields were parsed out correctly
    CPPUNIT_ASSERT(RestRequest::GET == request.getMethod());
    CPPUNIT_ASSERT(0 == request.getPath().compare(path));
    CPPUNIT_ASSERT(0 == request.getProtocol().compare(protocol));
    CPPUNIT_ASSERT(0 == request.getBody().compare(body));

    // Verify that the headers were set correctly
    map<string, string> headers;
    request.getHeaders(&headers);
    map<string, string>::iterator iter;
    for (iter = headers.begin(); iter != headers.end(); iter++)
    {
        if (0 == (iter->first).compare(headerName1))
        {
            CPPUNIT_ASSERT(0 == (iter->second).compare(headerValue1));
        }
        else if (0 == (iter->first).compare(headerName2))
        {
            CPPUNIT_ASSERT(0 == (iter->second).compare(headerValue2));
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
    }
}

void TestRestRequest::setUp(void)
{
    request = new RestRequest();
}

void TestRestRequest::tearDown(void)
{
    delete request;
}

//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( TestRestRequest );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
    ofstream xmlFileOut("cppTestRestRequest.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
