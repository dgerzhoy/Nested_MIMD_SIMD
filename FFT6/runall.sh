#!/bin/bash

P=./fft6
N=8192
C=2048
make clean all
#make

#./fft6 <Size> <CPU Threads> <0 = CPU kernel, 1 = GPU Kernel> <$C = cutoff (gpu only)>
$P $N 1 0  #CPU 1 Thread
$P $N 4 0  #CPU 4 Thread
$P $N 1 1 $C  #GPU 1 Thread
$P $N 4 1 $C  #GPU 1 Thread