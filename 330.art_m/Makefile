EXECUTABLES = art 

#CC=g++ -std=c++11
CC=g++-4.4

COMMON_INC = ../common
VPATH = ../common
C_SRC=$(wildcard ./*.c)
CPP_SRC=$(wildcard ./*.cpp) $(wildcard ${COMMON_INC}/*.cpp)
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I.

OPT=-O3
G=
#-g

CDEFS:=-DPTHREADS
#-DPROFILE_LOOPS 
#-DNULL_KERN 
#-DVERBOSE
# -DDEBUG

all: $(OBJ) $(EXECUTABLES)

%.co : %.c
	g++ -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -std=gnu++11 -c -o $@ $^

%.cppo : %.cpp
	g++ -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp -lOpenCL -std=gnu++11 -c -o $@ $^

art: $(OBJ)
	g++ -fpermissive $(OPT) $(G) -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp -lOpenCL -std=gnu++11

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ../common/*.o ../common/*.co ../common/*.cppo
