#!/bin/bash

#usage: ./runall.sh

make clean all
#make
./run.sh testhuge 1 0 |& tee CPU1
./run.sh testhuge 4 0 |& tee CPU4
./run.sh testhuge 1 3 |& tee FLEX1
./run.sh testhuge 4 3 |& tee FLEX4
