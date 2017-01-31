SRCS=CacheTrace.cpp TimeTrace.cpp Cycles.cpp Util.cpp
INCLUDES=CacheTrace.h TimeTrace.h Cycles.h Util.h
OBJECTS:=$(SRCS:.cpp=.o)


default: libPerfUtils.a

all: libPerfUtils.so libPerfUtils.a

libPerfUtils.so: $(SRCS) $(INCLUDES)
	g++  -O3 -c -fPIC -std=c++0x  $(SRCS)
	$(CC) -shared -Wl,-soname,libPerfUtils.so.0.0 -o $@ $(LDFLAGS) $(OBJECTS)
	ln -f -s $@ libPerfUtils.so.0.0

libPerfUtils.a: $(SRCS) $(INCLUDES)
	g++  -O3 -c -fPIC -std=c++0x  $(SRCS)
	ar -cvq libPerfUtils.a $(OBJECTS)

TimeTraceTest: TimeTraceTest.cpp
	g++ -o TimeTraceTest -std=c++0x  TimeTraceTest.cpp TimeTrace.cpp Cycles.cpp

clean:
	rm -f libPerfUtils.a TimeTraceTest *.o *.gch *.log libPerfUtils.so \
		libPerfUtils.so.0.0
