#!/bin/bash

#usage: ./runall.sh

N=16
M=501

make clean all

#./bots-sparlu <Size> <CPU Threads> <0 = CPU kernel, 2 = UNUSED, 3 = GPU Kernel>
./bots-sparlu -n $N -m $M -t 1 -k 0 |& tee CPU1
./bots-sparlu -n $N -m $M -t 4 -k 0 |& tee CPU4
./bots-sparlu -n $N -m $M -t 1 -k 3 |& tee FLEX1
./bots-sparlu -n $N -m $M -t 4 -k 3 |& tee FLEX4
