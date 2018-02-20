DESTDIR ?= .

CXX ?= g++
OBJECT_DIR = obj
SRC_DIR = src
INCLUDE_DIR = $(DESTDIR)/include
LIB_DIR = $(DESTDIR)/lib
CFLAGS=-O3 -DNDEBUG -fPIC -std=c++0x

# Stuff needed for make check
TOP := $(shell echo $${PWD-`pwd`})
ifndef CHECK_TARGET
CHECK_TARGET=$$(find $(SRC_DIR) '(' -name '*.h' -or -name '*.cc' ')' -not -path '$(TOP)/googletest/*' )
endif

OBJECT_NAMES := CacheTrace.o TimeTrace.o Cycles.o Util.o Stats.o mkdir.o

OBJECTS = $(patsubst %,$(OBJECT_DIR)/%,$(OBJECT_NAMES))
HEADERS= $(shell find src -name '*.h')
DEP=$(OBJECTS:.o=.d)

install: $(OBJECT_DIR)/libPerfUtils.a
	mkdir -p $(LIB_DIR) $(INCLUDE_DIR)/PerfUtils
	cp $(HEADERS) $(INCLUDE_DIR)/PerfUtils
	cp $(OBJECT_DIR)/libPerfUtils.a $(LIB_DIR)

$(OBJECT_DIR)/libPerfUtils.a: $(OBJECTS)
	ar cvr $@ $(OBJECTS)

$(OBJECT_DIR)/TimeTraceTest: $(OBJECT_DIR)/TimeTraceTest.o $(OBJECT_DIR)/libPerfUtils.a
	$(CXX) $(CFLAGS) -o $@ $^


-include $(DEP)

$(OBJECT_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJECT_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

check:
	scripts/cpplint.py --filter=-runtime/threadsafe_fn,-readability/streams,-whitespace/blank_line,-whitespace/braces,-whitespace/comments,-runtime/arrays,-build/include_what_you_use,-whitespace/semicolon,-build/include $(CHECK_TARGET)
	! grep '.\{81\}' $(SRC_DIR)/*.h $(SRC_DIR)/*.cc

clean:
	rm -rf $(LIB_DIR) $(INCLUDE_DIR) $(OBJECT_DIR)

.PHONY: install check clean
