EXECUTABLES = md 

CC=g++ -std=c++11
#CC=g++-4.4

COMMON_INC = ../common
VPATH = ../common
C_SRC=$(wildcard ./*.c) $(wildcard ./common/*.c) $(wildcard ./omp-tasks/*.c)
CPP_SRC= 
CPP_SRC= clSetup.cpp utils.cpp clSetup.cpp $(wildcard ./*.cpp) 
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I. -I./common -I./omp-tasks

NO_SSE_FLAGS=\
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
CDEFS:= $(CFLAGS) -DNDEBUG -DPTHREADS -DPTHREAD -DOCL 
#-DRESTART

all: $(OBJ) $(EXECUTABLES)

#-std=gnu++11 
%.co : %.c
	$(CC) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -c -o $@ $^

%.cppo : %.cpp
	$(CC) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -c -o $@ $^

md: $(OBJ)
	$(CC) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp -lOpenCL

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ../common/*.o ../common/*.co ../common/*.cppo
