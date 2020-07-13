#!/bin/bash

P=./fft6
N=8192
C=2048
make clean all
#make
$P $N 1 0 |& tee CPU1
$P $N 4 0 |& tee CPU4_
$P $N 1 1 $C |& tee FLEX1
$P $N 4 1 $C |& tee FLEX4

grep -a "Time Total" *_cleanTest

#make clean all CFLAGS=-DPERLOOP
#$P $N 1 0 $C |& tee CPU1_PERLOOP
#$P $N 1 3 $C |& tee FLEX1_PERLOOP
