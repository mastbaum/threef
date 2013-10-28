# Optimization flags
ifeq ($(OPTIMIZE),1)
$(warning *** Generating optimized build! CUDA error checking is OFF! ***)
OPT_CFLAGS = -O2
OPT_NVCCFLAGS = 
else
# Debug mode is default
OPT_CFLAGS = -g
OPT_NVCCFLAGS = -DDEBUG 
endif

INCLUDE = -Iinclude -I$(ROOTSYS)/include -Icontrib/hemi -I/usr/local/cuda/include
CFLAGS = -DVERBOSE=true $(OPT_CFLAGS) $(INCLUDE)
GCCFLAGS = -Wall -Werror -Wno-unused-variable -ftrapv -fdiagnostics-show-option  # -Wunused-variable errors with HEMI macros
NOT_NVCC_CFLAGS = -fPIC
NVCCFLAGS = -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=\"sm_35,compute_35\" -use_fast_math $(OPT_NVCCFLAGS)
ROOTLIBS =  -lCore -lCint -lRIO -lMathCore -lHist -lGpad -lTree -lTree -lGraf -lm -lPhysics -lMinuit2
LFLAGS = -L$(ROOTSYS)/lib $(ROOTLIBS) -L/opt/local/lib -lhdf5 -lhdf5_hl

# Mac hacks!
ARCH = $(shell uname)

ifndef CUDA_ROOT
$(warning *** CUDA_ROOT is not set, defaulting to CPU-only build ***)
GCC = g++ $(GCCFLAGS) $(NOT_NVCC_CFLAGS) -DHEMI_CUDA_DISABLE
CUDACC = $(CC)
CC = $(GCC)
else
GCC = g++ $(GCCFLAGS) $(NOT_NVCC_CFLAGS)
	ifeq ($(ARCH), Darwin)
		NVCCFLAGS := -m64 $(NVCCFLAGS)
		CUDA_LIBDIR = $(CUDA_ROOT)/lib
	else
		CUDA_LIBDIR = $(CUDA_ROOT)/lib64
	endif
CUDA_LFLAGS = -L$(CUDA_LIBDIR) -lcudart -lcurand
CC = $(CUDA_ROOT)/bin/nvcc $(NVCCFLAGS) -I$(CUDA_ROOT)/include
#CUDACC = $(CC) -x cu #this causes compiler errors when making the shared library
CUDACC = $(CC)
CC += --compiler-options "$(GCCFLAGS) -Wno-unused-function -fPIC"
endif

OBJ_DIR = build
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)

INCLUDES = $(wildcard src/*.h)
INCLUDES_DST = $(addprefix include/, $(notdir $(INCLUDES)))

# For unit test suit
#SXMC_NO_MAIN_FUNCTION_OBJECTS = $(filter-out build/sensitivity.o, $(OBJECTS) build/mcmc.o build/nll_kernels.o build/pdfz.o)
#TEST_SOURCES = $(wildcard test/*.cpp)
#TEST_OBJECTS = $(TEST_SOURCES:test/%.cpp=$(OBJ_DIR)/test/%.o)

#EXE = bin/test
LIBRARY = lib/libFFF.so

ifndef ROOTSYS
$(error ROOTSYS is not set)
endif

all: dirs includes $(OBJECTS) $(LIBRARY)

dirs:
	@test -d include || mkdir include
	@test -d build || mkdir build
	@test -d lib || mkdir lib

.PHONY: includes

includes: dirs $(INCLUDES_DST)

include/%: %
	cp $^ include

clean:
	-$(RM) include/* $(OBJ_DIR)/* $(LIBRARY)

vpath %.h src



$(OBJ_DIR)/%.o: src/%.cpp $(INCLUDES_DST)
	$(CUDACC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/tmath.o: src/tmath.cpp $(INCLUDES_DST)
	$(GCC) -c -o $@ $< $(CFLAGS)

$(LIBRARY): $(OBJECTS)
	$(CUDACC) -shared -o $@ $^ $(LFLAGS)

