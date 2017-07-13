CC=g++
OBJECT_DIR = obj
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
CFLAGS=-O3 -fPIC -std=c++0x

# Stuff needed for make check
TOP := $(shell echo $${PWD-`pwd`})
ifndef CHECK_TARGET
CHECK_TARGET=$$(find $(SRC_DIR) '(' -name '*.h' -or -name '*.cc' ')' -not -path '$(TOP)/googletest/*' )
endif

OBJECT_NAMES := CacheTrace.o TimeTrace.o Cycles.o Util.o

OBJECTS = $(patsubst %,$(OBJECT_DIR)/%,$(OBJECT_NAMES))
HEADERS= $(shell find src -name '*.h')
DEP=$(OBJECTS:.o=.d)

install: $(OBJECT_DIR)/libPerfUtils.so $(OBJECT_DIR)/libPerfUtils.a
	mkdir -p lib include/PerfUtils
	cp $(HEADERS) include/PerfUtils
	cp $(OBJECT_DIR)/libPerfUtils.a $(OBJECT_DIR)/libPerfUtils.so lib

$(OBJECT_DIR)/libPerfUtils.so: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libPerfUtils.so.0.0 -o $@ $(LDFLAGS) $(OBJECTS)
	ln -f -s $@ $(OBJECT_DIR)/libPerfUtils.so.0.0

$(OBJECT_DIR)/libPerfUtils.a: $(OBJECTS)
	ar -cvq $@ $(OBJECTS)

-include $(DEP)

$(OBJECT_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJECT_DIR)
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJECT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

check:
	scripts/cpplint.py --filter=-runtime/threadsafe_fn,-readability/streams,-whitespace/blank_line,-whitespace/braces,-whitespace/comments,-runtime/arrays,-build/include_what_you_use,-whitespace/semicolon $(CHECK_TARGET)
	! grep '.\{81\}' $(SRC_DIR)/*.h $(SRC_DIR)/*.cpp

clean:
	rm -rf lib/ include/ obj/
