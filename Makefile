DESTDIR ?= .

CXX ?= g++
CC ?= gcc
OBJECT_DIR = obj
SRC_DIR = src
WRAPPER_DIR = cwrapper
INCLUDE_DIR = $(DESTDIR)/include
LIB_DIR = $(DESTDIR)/lib
CXXFLAGS=-O3 -DNDEBUG -fPIC -std=c++11 -pthread
CFLAGS=-O3 -DNDEBUG -fPIC -std=gnu99
INCLUDE=-I$(SRC_DIR)

# Stuff needed for make check
TOP := $(shell echo $${PWD-`pwd`})
ifndef CHECK_TARGET
CHECK_TARGET=$$(find $(SRC_DIR) $(WRAPPER_DIR) '(' -name '*.h' -or -name '*.cc' ')' -not -path '$(TOP)/googletest/*' )
endif

OBJECT_NAMES := CacheTrace.o TimeTrace.o Cycles.o Util.o Stats.o Perf.o mkdir.o timetrace_wrapper.o cycles_wrapper.o perf_wrapper.o

OBJECTS = $(patsubst %,$(OBJECT_DIR)/%,$(OBJECT_NAMES))
HEADERS= $(shell find $(SRC_DIR) $(WRAPPER_DIR) -name '*.h')
DEP=$(OBJECTS:.o=.d)

install: $(OBJECT_DIR)/libPerfUtils.a
	mkdir -p $(LIB_DIR) $(INCLUDE_DIR)/PerfUtils
	cp $(HEADERS) $(INCLUDE_DIR)/PerfUtils
	cp $(OBJECT_DIR)/libPerfUtils.a $(LIB_DIR)

$(OBJECT_DIR)/libPerfUtils.a: $(OBJECTS)
	ar cvr $@ $(OBJECTS)

$(OBJECT_DIR)/TimeTraceTest: $(OBJECT_DIR)/TimeTraceTest.o $(OBJECT_DIR)/libPerfUtils.a
	$(CXX) $(CXXFLAGS) -o $@ $^

-include $(DEP)

$(OBJECT_DIR)/%.d: $(WRAPPER_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(INCLUDE) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

$(OBJECT_DIR)/%.o: $(WRAPPER_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR)/%.d: $(WRAPPER_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(INCLUDE) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) > $@

$(OBJECT_DIR)/%.o: $(WRAPPER_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@

$(OBJECT_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(INCLUDE) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) > $@

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

check:
	scripts/cpplint.py --filter=-runtime/threadsafe_fn,-readability/streams,-whitespace/blank_line,-whitespace/braces,-whitespace/comments,-runtime/arrays,-build/include_what_you_use,-whitespace/semicolon,-build/include $(CHECK_TARGET)
	! grep '.\{81\}' $(SRC_DIR)/*.h $(SRC_DIR)/*.cc
################################################################################
# Test targets
GTEST_DIR=../googletest/googletest
GMOCK_DIR=../googletest/googlemock
TEST_LIBS=-Lobj/ -lPerfUtils $(OBJECT_DIR)/libgtest.a  $(OBJECT_DIR)/libgmock.a
INCLUDE+=-I${GTEST_DIR}/include -I${GMOCK_DIR}/include

test: $(OBJECT_DIR)/UtilTest $(OBJECT_DIR)/PerfTest $(OBJECT_DIR)/StatsTest \
	  $(OBJECT_DIR)/cycles_wrapper_test  $(OBJECT_DIR)/perf_wrapper_test  $(OBJECT_DIR)/timetrace_wrapper_test
	$(OBJECT_DIR)/UtilTest
	$(OBJECT_DIR)/PerfTest
	$(OBJECT_DIR)/StatsTest
	$(OBJECT_DIR)/cycles_wrapper_test
	$(OBJECT_DIR)/perf_wrapper_test
	$(OBJECT_DIR)/timetrace_wrapper_test

$(OBJECT_DIR)/UtilTest: $(OBJECT_DIR)/UtilTest.o $(OBJECT_DIR)/libgtest.a  $(OBJECT_DIR)/libgmock.a \
						$(OBJECT_DIR)/libPerfUtils.a
	$(CXX) $(INCLUDE) $(CXXFLAGS) $< $(GTEST_DIR)/src/gtest_main.cc $(TEST_LIBS) $(LIBS)  -o $@

$(OBJECT_DIR)/PerfTest: $(OBJECT_DIR)/PerfTest.o $(OBJECT_DIR)/libgtest.a  $(OBJECT_DIR)/libgmock.a \
						$(OBJECT_DIR)/libPerfUtils.a
	$(CXX) $(INCLUDE) $(CXXFLAGS) $< $(GTEST_DIR)/src/gtest_main.cc $(TEST_LIBS) $(LIBS)  -o $@

$(OBJECT_DIR)/StatsTest: $(OBJECT_DIR)/StatsTest.o $(OBJECT_DIR)/libgtest.a  $(OBJECT_DIR)/libgmock.a \
						$(OBJECT_DIR)/libPerfUtils.a
	$(CXX) $(INCLUDE) $(CXXFLAGS) $< $(GTEST_DIR)/src/gtest_main.cc $(TEST_LIBS) $(LIBS)  -o $@

$(OBJECT_DIR)/libgtest.a:
	$(CXX) -I${GTEST_DIR}/include -I${GTEST_DIR} \
        -pthread -c ${GTEST_DIR}/src/gtest-all.cc \
        -o $(OBJECT_DIR)/gtest-all.o
	ar -rv $(OBJECT_DIR)/libgtest.a $(OBJECT_DIR)/gtest-all.o

$(OBJECT_DIR)/libgmock.a:
	$(CXX) -I${GTEST_DIR}/include -I${GTEST_DIR} \
		-I${GMOCK_DIR}/include -I${GMOCK_DIR} \
        -pthread -c ${GTEST_DIR}/src/gtest-all.cc \
        -o $(OBJECT_DIR)/gmock-all.o
	ar -rv $(OBJECT_DIR)/libgmock.a $(OBJECT_DIR)/gmock-all.o

$(OBJECT_DIR)/timetrace_wrapper_test: $(OBJECT_DIR)/timetrace_wrapper_test.o $(OBJECT_DIR)/libPerfUtils.a
	$(CC) $(CFLAGS) -lstdc++ -o $@ $^

$(OBJECT_DIR)/cycles_wrapper_test: $(OBJECT_DIR)/cycles_wrapper_test.o $(OBJECT_DIR)/libPerfUtils.a
	$(CC) $(CFLAGS) -lstdc++ -o $@ $^

$(OBJECT_DIR)/perf_wrapper_test: $(OBJECT_DIR)/perf_wrapper_test.o $(OBJECT_DIR)/libPerfUtils.a
	$(CC) $(CFLAGS) -lstdc++ -o $@ $^ -lm


################################################################################
clean:
	rm -rf $(LIB_DIR) $(INCLUDE_DIR) $(OBJECT_DIR)

.PHONY: install check clean
