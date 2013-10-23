CFLAGS = -I. $(shell root-config --cflags)
LFLAGS = $(shell root-config --libs)
CC = g++
OBJ_DIR = build
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)
INCLUDES = $(wildcard src/*.h)
INCLUDES_DST = $(addprefix include/, $(notdir $(INCLUDES)))
EXE = bin/toy

ifndef ROOTSYS
$(error ROOTSYS is not set)
endif

all: dirs includes $(OBJECTS) $(EXE)

clean:
	-$(RM) include/* build/* $(EXE)

dirs:
	test -d include || mkdir include
	test -d build || mkdir build
	test -d bin || mkdir bin

.PHONY: includes

includes: dirs $(INCLUDES_DST)

include/%: %
	cp $^ include

vpath %.h src

$(OBJ_DIR)/%.o: src/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

