#!/bin/bash

#usage: ./runall.sh

#28 cache test
#30 test
#32 train
#35 131235
#36 262307
#41 ref 1048740
N=41

#The grid options in grid_def change the way the work is split up on the CPU

#rm grid_def.h
#echo "#define NO_GRID" > grid_def.h
#echo "#define NO_GRID_STRIPED" >> grid_def.h
#make clean all CFLAGS="-DNO_GRID -DNO_GRID_STRIPED"
#./smithwaterman $N 4 3 |& tee FLEX4_FUSE_STRIPE
#./smithwaterman $N 1 3 |& tee FLEX1_FUSE_STRIPE
#./smithwaterman $N 4 0 |& tee CPU4_FUSE_STRIPE
#./smithwaterman $N 1 0 |& tee CPU1_FUSE_STRIPE

#loop fusion (the original) for the CPU, GPU is still fissioned
rm grid_def.h
touch grid_def.h
make clean all
./smithwaterman $N 4 3 |& tee FLEX4_FUSE_GRID
./smithwaterman $N 1 3 |& tee FLEX1_FUSE_GRID
# ./smithwaterman $N 6 0 |& tee CPU6_FUSE_GRID
# ./smithwaterman $N 8 0 |& tee CPU8_FUSE_GRID
./smithwaterman $N 4 0 |& tee CPU4_FUSE_GRID
./smithwaterman $N 1 0 |& tee CPU1_FUSE_GRID
#
#
#rm grid_def.h
#echo "#define NO_GRID" > grid_def.h
#make clean all CFLAGS="-DNO_GRID"
#./smithwaterman $N 4 3 |& tee FLEX4_FUSE_NOGRID
#./smithwaterman $N 1 3 |& tee FLEX1_FUSE_NOGRID
#./smithwaterman $N 4 0 |& tee CPU4_FUSE_NOGRID
#./smithwaterman $N 1 0 |& tee CPU1_FUSE_NOGRID

#Loop Fission for the CPU as well

#rm grid_def.h
#echo "#define NO_GRID" > grid_def.h
#make clean all CFLAGS="-DLOOP_FISSION -DNO_GRID"
#./smithwaterman $N 1 0 |& tee CPU1_FISS_NOGRID
#./smithwaterman $N 4 0 |& tee CPU4_FISS_NOGRID
#./smithwaterman $N 1 3 |& tee FLEX1_FISS_NOGRID
#./smithwaterman $N 4 3 |& tee FLEX4_FISS_NOGRID

#make
#rm CPU1 CPU4 FLEX1 FLEX4
#./smithwaterman $N 1 0 |& tee CPU_STRIPED_FISSION1
#./smithwaterman $N 4 0 |& tee CPU_STRIPED_FISSION4
#./smithwaterman $N 1 3 |& tee FLEX_STRIPED1
#./smithwaterman $N 4 3 |& tee FLEX_STRIPED4

#cp CPU1 CPU1_${N}
#cp CPU4 CPU4_${N}
#cp FLEX1 FLEX1_${N}
#cp FLEX4 FLEX4_${N}
