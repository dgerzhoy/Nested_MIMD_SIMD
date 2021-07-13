EXECUTABLES = convert

#CC=g++ -std=gnu++11
CC=g++-4.4

COMMON_INC = ../common
VPATH = ../common
C_SRC=$(filter-out ImageValidator.c, $(wildcard *.c))
CPP_SRC= $(wildcard ../common/*.cpp) $(wildcard ./*.cpp)
#CPP_SRC= magick_resize.cpp utilities_convert.cpp timers.c
SRC= $(CPP_SRC) $(C_SRC)

C_OBJ=$(C_SRC:.c=.co)
CPP_OBJ=$(CPP_SRC:.cpp=.cppo)
OBJ=$(CPP_OBJ) $(C_OBJ)

INC= -I.# -I./wand -I./magick

OPT=-O3
CFLAGS:=
CDEFS:=-DOCL -DINNER_CORE $(CFLAGS)
OCL=-lOpenCL
#OCL=
STATIC=
#STATIC=-static -static-libgcc

all: $(OBJ) $(EXECUTABLES)

%.co : %.c
	$(CC) $(OPT) -g -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL) $(STATIC)-c -o $@ $^

%.cppo : %.cpp
	$(CC) $(OPT) -g -w $(CDEFS) $(INC) -I$(COMMON_INC) -lm -lrt -fopenmp $(OCL) $(STATIC) -c -o $@ $^

convert: $(OBJ)
	$(CC) $(OPT) -g -w $(CDEFS) $(INC) -I$(COMMON_INC) $(CL_CFLAGS) $(CL_LDFLAGS)  -o$@ $^ -lm -lrt -fopenmp $(OCL) $(STATIC)

clean:
	rm -f $(EXECUTABLES) *.co *.cppo *.o ../common/*.o ../common/*.co ../common/*.cppo
