EXECUTABLES = bots-alignment

#CC=g++ -std=gnu++11
CC=g++-4.4
#CC=g++-8 -std=gnu++11

OMP_TASKS=./omp-tasks/alignment/alignment_for

COMMON_INC = ../common
VPATH = ../common ${OMP_TASKS}
C_SRC=$(wildcard ./common/*.c) $(wildcard ${OMP_TASKS}/*.c)
CPP_SRC= $(wildcard ../common/*.cpp) $(wildcard ./*.cpp) $(wildcard ${OMP_TASKS}/*.cpp) $(wildcard ./common/*.cpp)
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I. -I./common -I${OMP_TASKS}

#NO_SSE_FLAGS=\
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

FLUSHOPT=
#-mclflushopt

CDEFS:= $(CFLAGS) -DPTHREAD -DLOOP_FISSION -DOCL -DBOTH -DLOOP_FUSION 
#-DELIM_HTMP
#-DFLUSH_HELPER -DWAIT_SUSPEND 
#-DREAD_FLUSH
#-DPREV_OPT 
#-DRESTART 
#-DCHAR_OPT -DPREV_OPT 
#-DLOOP_FUSION 
#-DFLEX_HYBRID 
#-DPREV_OPT
#-DLOOP_FUSION 
OCL=-lOpenCL
#OCL=
STATIC=
#STATIC=-static -static-libgcc


all: $(OBJ) $(EXECUTABLES)

#-std=gnu++11
%.co : %.c
	$(CC) -fpermissive $(FLUSHOPT) $(NO_SSE_FLAGS) $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL) $(STATIC)  -c -o $@ $^

%.cppo : %.cpp
	$(CC) -fpermissive $(FLUSHOPT) $(NO_SSE_FLAGS) $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL) $(STATIC) -c -o $@ $^

bots-alignment: $(OBJ)
	$(CC) -fpermissive $(FLUSHOPT) $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp $(OCL) $(STATIC)

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ./common/*.o ./common/*.co ./common/*.cppo ${OMP_TASKS}/*.o ${OMP_TASKS}/*.co ${OMP_TASKS}/*.cppo ../common/*.o ../common/*.co ../common/*.cppo
