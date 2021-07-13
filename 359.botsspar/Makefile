EXECUTABLES = bots-sparlu

CC=g++ -std=gnu++11
#CC=g++-4.4

OMP_TASKS=./omp-tasks/sparselu/sparselu_single

COMMON_INC = ../common
VPATH = ../common ${OMP_TASKS}
C_SRC=$(wildcard ./common/*.c) $(wildcard ${OMP_TASKS}/*.c)
CPP_SRC= $(wildcard ../common/*.cpp) $(wildcard ./*.cpp) $(wildcard ${OMP_TASKS}/*.cpp) $(wildcard ./common/*.cpp)
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I. -I./common -I${OMP_TASKS}

NO_SSE_FLAGS=\
\
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
CDEFS:= -DPTHREAD 
#-DBMOD_UNROLL 

#-DVERBOSE
#-DCHECK_COMM 
#-DFLEX_HYBRID 
#-DBDIV_UNROLL -DFWD_UNROLL
#-DBMOD_2D -ffloat-store
#-DSKIP_LU0
#-DVERBOSE
#-DCHECK_COMM
#-DCHECK_MAT
#-DBMOD_2D

all: $(OBJ) $(EXECUTABLES)

%.co : %.c
	$(CC) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -c -o $@ $^

#./omp-tasks/sparselu.cppo : ./omp-tasks/sparselu.cpp
#	$(CC)  $(NO_SSE_FLAGS) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -c -o $@ $^

%.cppo : %.cpp
	$(CC)  -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -c -o $@ $^

bots-sparlu: $(OBJ)
	$(CC) -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp -lOpenCL

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ./common/*.o ./common/*.co ./common/*.cppo ${OMP_TASKS}/*.o ${OMP_TASKS}/*.co ${OMP_TASKS}/*.cppo ../common/*.o ../common/*.co ../common/*.cppo
