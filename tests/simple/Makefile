CFLAGS = $(shell root-config --cflags) -I../../include
LFLAGS = $(shell root-config --libs) -lMinuit2 -L../../lib -lFFF
CCFLAGS = -Wno-deprecated-declarations
CC = g++
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
EXE = test

all: $(OBJECTS) $(EXE)

clean:
	-$(RM) *.o $(EXE)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJECTS)
	$(CC) $(CCFLAGS) -o $@ $^ $(CFLAGS) $(LFLAGS)
