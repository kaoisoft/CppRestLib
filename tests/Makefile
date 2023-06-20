CXX = g++
INCLUDES= -I./ -I../
CXXFLAGS = -g $(INCLUDES)
SRCM= ../RestRequest.cpp
OBJM = $(SRCM:.cpp=.o)
LINKFLAGS= -lcppunit

testRestRequest: TestRestRequest.cpp $(OBJM)
	$(CXX) $(CXXFLAGS) -o $@ TestRestRequest.cpp $(OBJM) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

# Default compile

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

