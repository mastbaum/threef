CFLAGS = -Iinclude $(shell root-config --cflags)
LFLAGS = $(shell root-config --libs) -lMinuit2
CCFLAGS = -Wno-deprecated-declarations
CC = g++
OBJ_DIR = build
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)
INCLUDES = $(wildcard src/*.h)
INCLUDES_DST = $(addprefix include/, $(notdir $(INCLUDES)))
LIB = lib/libaurore.so

ifndef ROOTSYS
$(error ROOTSYS is not set)
endif

all: dirs includes $(OBJECTS) $(LIB)

clean:
	-$(RM) include/aurore/* build/* lib/* $(EXE)

dirs:
	test -d include/aurore || mkdir -p include/aurore
	test -d build || mkdir build
	#test -d bin || mkdir bin
	test -d lib || mkdir lib

.PHONY: includes

includes: dirs $(INCLUDES_DST)

include/%: %
	cp $^ include/aurore/

vpath %.h src

$(OBJ_DIR)/%.o: src/%.cpp
	$(CC) $(CCFLAGS) -c -o $@ $< $(CFLAGS)

$(LIB): $(OBJECTS)
	$(CC) $(CCFLAGS) -shared -o $@ $^ $(CFLAGS) $(LFLAGS)

#$(EXE): $(OBJECTS)
#	$(CC) $(CCFLAGS) -o $@ $^ $(CFLAGS) $(LFLAGS)

