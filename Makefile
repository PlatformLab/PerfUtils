SRCS=CacheTrace.cpp TimeTrace.cpp Cycles.cpp Util.cpp
INCLUDES=CacheTrace.h TimeTrace.h Cycles.h Util.h
OBJECTS:=$(SRCS:.cpp=.o)



all: libPerfUtils.a

libPerfUtils.a: $(SRCS) $(INCLUDES)
	g++  -O3 -c -fPIC -std=c++0x  $(SRCS)
	ar -cvq libPerfUtils.a $(OBJECTS)

TTTest: TTTest.cpp 
	g++ -o TTTest -std=c++0x  TTTest.cpp TimeTrace.cpp Cycles.cpp

clean:
	rm -f libPerfUtils.a TTTest *.o *.gch *.log
