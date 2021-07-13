EXECUTABLES = smithwaterman

#CC=g++ -std=c++11
CC=g++-4.4
#CC=icpc -std=c++11


COMMON_INC = ../common
VPATH = ../common
C_SRC=$(wildcard ./*.c) $(wildcard ./common/*.c)
CPP_SRC= 
CPP_SRC=  $(wildcard ../common/*.cpp) $(wildcard ./*.cpp)
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I. -I./common -I./omp-tasks

verbosity=
OCL_LIB=-lOpenCL
#/opt/intel/opencl/libOpenCL.so.1
#OCL_LIB=-lIntelOpenCL
#OCL_LIB=-l/opt/intel/opencl/libOpenCL.so
#OCL_LIB=-L/opt/intel/opencl/ -lOpenCL
#OCL_LIB=-locl-icd

NO_SSE_FLAGS=
#\
-mno-mmx \
-mno-sse \
-mno-sse2 \
-mno-sse3 \
-mno-ssse3 \
-mno-sse4.1 \
-mno-sse4.2 \
-mno-sse4 \
-mno-sse4a \
-mno-sse5

OPT=-O3
G=
#-g

CFLAGS:=
CDEFS:=-DNDEBUG -DPTHREADS -DPTHREAD -DMSR_ROI -DOCL  -DCORE_OPT -DOVERLAP -DCPU_nOCL $(CFLAGS) -DFLEX_HYBRID -DVPREV
#-DMSR_KERNEL0 -DMSR_KERNEL1 -DMSR_KERNEL2
#-DRESTART
#-DLOOP_FISSION 
#-DUNROLL 
#-DNO_GRID -DNO_GRID_STRIPED -DLOOP_FISSION

all: $(OBJ) $(EXECUTABLES)

#-std=gnu++11 
%.co : %.c
	$(CC) $(verbosity) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL_LIB) -c -o $@ $^

%.cppo : %.cpp
	$(CC) $(verbosity) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL_LIB) -c -o $@ $^

smithwaterman: $(OBJ)
	$(CC) $(verbosity) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp $(OCL_LIB) 

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ../common/*.o ../common/*.co ../common/*.cppo
