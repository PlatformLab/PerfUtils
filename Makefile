

all: libPerfUtils.a

libPerfUtils.a: 
	g++  -O3 -c  -std=c++0x  TimeTrace.cpp Cycles.cpp Util.cpp
	ar -cvq libPerfUtils.a TimeTrace.o Cycles.o Util.o

TTTest: TTTest.cpp 
	g++ -o TTTest -std=c++0x  TTTest.cpp TimeTrace.cpp Cycles.cpp

clean:
	rm -f libPerfUtils.a TTTest *.o *.gch *.log
