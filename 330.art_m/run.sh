#!/bin/bash

KERNEL=$3
CUTOFF=$4

SCAN=c756hel.in
T1=a10.img
T2=hc.img

if [ "$1" = "ref" ]; then
        echo "ref"

STRIDE=1
STARTX=110
STARTY=220
ENDX=172
ENDY=260
OBJS=1000
ITERS=16776

elif [ "$1" = "testhuge" ]; then
        echo "testhuge"

STRIDE=2
STARTX=130
STARTY=220
ENDX=150
ENDY=230
OBJS=1000
ITERS=544

elif [ "$1" = "test" ]; then
        echo "testhuge"

STRIDE=2
STARTX=130
STARTY=220
ENDX=150
ENDY=230
OBJS=1000
ITERS=544


fi


#./art -scanfile $SCAN -trainfile1 $T1 -trainfile2 $T2 -stride $STRIDE -startx $STARTX -starty $STARTY -endx $ENDX -endy $ENDY -objects $OBJS -preTrain 1 -preTrainfile ./trainFile_$1_$2.bin -numThreads $2
cmd="./art -scanfile $SCAN -trainfile1 $T1 -trainfile2 $T2 -stride $STRIDE -startx $STARTX -starty $STARTY -endx $ENDX -endy $ENDY -objects $OBJS -numThreads $2 -iters $ITERS -kernelType $KERNEL"
# -cutoff $CUTOFF
echo $cmd
time ./art -scanfile $SCAN -trainfile1 $T1 -trainfile2 $T2 -stride $STRIDE -startx $STARTX -starty $STARTY -endx $ENDX -endy $ENDY -objects $OBJS -numThreads $2  -iters $ITERS -kernelType $KERNEL
#-cutoff $CUTOFF


