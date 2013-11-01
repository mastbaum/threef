CFLAGS = -Iinclude $(shell root-config --cflags)
LFLAGS = $(shell root-config --libs) -lMinuit2
CCFLAGS = -Wno-deprecated-declarations
CC = g++
OBJ_DIR = build
SOURCES := $(shell find ./src -name '*.cpp')
OBJECTS = $(SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)
INCLUDES := $(shell find ./src -name '*.h')
INCLUDES_DST = $(addprefix include/, $(subst ./src/,, $(INCLUDES)))# $(notdir $(INCLUDES))))
LIB = lib/libaurore.so

ifndef ROOTSYS
$(error ROOTSYS is not set)
endif

all: dirs includes $(OBJECTS) $(LIB)

clean:
	-$(RM) -r include/aurore/* build/* lib/* $(EXE)

dirs:
	@test -d include/aurore || mkdir -p include/aurore
	@test -d build || mkdir build
	@test -d lib || mkdir lib

.PHONY: includes

includes: dirs $(INCLUDES_DST)

include/%: %
	@test -d $(dir $(subst src,include/aurore,$^)) || mkdir -p $(dir $(subst src,include/aurore,$^))
	cp -p $^ $(addprefix include/aurore/, $(subst src/,, $^))

vpath %.h src
vpath samplers/%.h src/samplers

$(OBJ_DIR)/%.o: src/%.cpp
	$(CC) $(CCFLAGS) -c -o $@ $< $(CFLAGS)

$(LIB): $(OBJECTS)
	$(CC) $(CCFLAGS) -shared -o $@ $^ $(CFLAGS) $(LFLAGS)

#$(EXE): $(OBJECTS)
#	$(CC) $(CCFLAGS) -o $@ $^ $(CFLAGS) $(LFLAGS)

