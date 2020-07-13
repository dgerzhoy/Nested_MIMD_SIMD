#!/bin/bash

N=4096
make clean all
#./md <Size> <CPU Threads> <0 = CPU kernel, 1 = GPU Kernel>
./md $N 1 0 #CPU 1 Thread
./md $N 4 0 #CPU 4 Thread
./md $N 1 1 #GPU 1 Thread
./md $N 4 1 #GPU 1 Thread
