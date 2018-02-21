DESTDIR ?= .

CXX ?= g++
CC ?= gcc
OBJECT_DIR = obj
SRC_DIR = src
WRAPPER_DIR = cwrapper
INCLUDE_DIR = $(DESTDIR)/include
LIB_DIR = $(DESTDIR)/lib
CXXFLAGS=-O3 -DNDEBUG -fPIC -std=c++0x
CFLAGS=-O3 -DNDEBUG -fPIC -std=gnu99
INCLUDE=-I$(SRC_DIR)

# Stuff needed for make check
TOP := $(shell echo $${PWD-`pwd`})
ifndef CHECK_TARGET
CHECK_TARGET=$$(find $(SRC_DIR) $(WRAPPER_DIR) '(' -name '*.h' -or -name '*.cc' ')' -not -path '$(TOP)/googletest/*' )
endif

OBJECT_NAMES := CacheTrace.o TimeTrace.o Cycles.o Util.o Stats.o mkdir.o timetrace_wrapper.o

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

$(OBJECT_DIR)/timetrace_wrapper_test: $(OBJECT_DIR)/timetrace_wrapper_test.o $(OBJECT_DIR)/libPerfUtils.a
	$(CC) $(CFLAGS) -lstdc++ -o $@ $^

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

clean:
	rm -rf $(LIB_DIR) $(INCLUDE_DIR) $(OBJECT_DIR)

.PHONY: install check clean
