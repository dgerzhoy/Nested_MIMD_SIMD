#!/bin/bash

#./run.sh <Size> <CPU Threads> <0 = CPU kernel, 3 = GPU Kernel, 4 = UNROLLED GPU> <UNROLL FACTOR>

#CPU RUNS
make clean all
./run.sh ref 1 0 |& tee $2/CPU1_run$1
./run.sh ref 4 0 |& tee $2/CPU4_run$1

#Defines in the ELIM.h File change how loop fission is done
rm ELIM.h
touch ELIM.h
../tools/reset_gpu.sh
./run.sh ref 1 3 |& tee $2/FLEX1_pre_run$1
../tools/reset_gpu.sh
./run.sh ref 4 3 |& tee $2/FLEX4_pre_run$1
../tools/reset_gpu.sh
./run.sh ref 1 4 150 |& tee $2/UROLL1_pre_run$1
../tools/reset_gpu.sh
./run.sh ref 4 4 150 |& tee $2/UROLL4_pre_run$1

echo "#define ELIM_HTMP" > ELIM.h
make clean all CFLAGS="-DELIM_HTMP"
../tools/reset_gpu.sh
./run.sh ref 1 3 |& tee $2/FLEX1_post_run$1
../tools/reset_gpu.sh
./run.sh ref 4 3 |& tee $2/FLEX4_post_run$1
../tools/reset_gpu.sh
./run.sh ref 1 4 150 |& tee $2/UROLL1_post_run$1
../tools/reset_gpu.sh
./run.sh ref 4 4 150 |& tee $2/UROLL4_post_run$1

#for u in 1 2 5 10 15 30; do
#for u in 30 60 100 150 300; do

	#./run.sh ref 1 4 $u |& tee UROLL1_300_$u
	#./run.sh ref 4 4 $u |& tee UROLL4_300_$u
