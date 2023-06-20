CXX = g++
INCLUDES= -I$(HOME)/include
CXXFLAGS = -g $(INCLUDES)
OBJS = main.o
LINKFLAGS= -L$(HOME)/lib -llog4cxx -lCppRestLib

all: $(OBJS) server_example

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
	
server_example: main.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LINKFLAGS)

