#!/bin/bash

N=4096
make clean all
#make
#./md $N 1 0 |& tee CPU1_cleanTest
#./md $N 4 0 |& tee CPU4_cleanTest
# ./md $N 1 1 |& tee FLEX1_cleanTest
# ./md $N 4 1 |& tee FLEX4_cleanTest
./md $N 1 0
./md $N 4 0
./md $N 1 1
./md $N 4 1

# grep -a "Time Total" *cleanTest
