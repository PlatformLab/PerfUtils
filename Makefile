CC=g++
OBJECT_DIR = obj
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
CFLAGS=-O3 -fPIC -std=c++0x

OBJECT_NAMES := CacheTrace.o TimeTrace.o Cycles.o Util.o

OBJECTS = $(patsubst %,$(OBJECT_DIR)/%,$(OBJECT_NAMES))
HEADERS= $(shell find src -name '*.h')

install: $(OBJECT_DIR)/libPerfUtils.so $(OBJECT_DIR)/libPerfUtils.a
	mkdir -p lib include/PerfUtils
	cp $(HEADERS) include/PerfUtils
	cp $(OBJECT_DIR)/libPerfUtils.a $(OBJECT_DIR)/libPerfUtils.so lib

$(OBJECT_DIR)/libPerfUtils.so: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libPerfUtils.so.0.0 -o $@ $(LDFLAGS) $(OBJECTS)
	ln -f -s $@ $(OBJECT_DIR)/libPerfUtils.so.0.0

$(OBJECT_DIR)/libPerfUtils.a: $(OBJECTS)
	ar -cvq $@ $(OBJECTS)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJECT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

clean:
	rm -rf lib/ include/ obj/
