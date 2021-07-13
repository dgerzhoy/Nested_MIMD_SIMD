#!/bin/bash

#AUTO-GENERATED SCRIPT TO RECONSTRUCT 358.botsalgn

#Usage: source reconstruct_358_botsalgn.sh <spec root> <spec benchmark dir> <destination dir> <src_alt>
#e.g.: source reconstruct_358_botsalgn.sh $SPEC ./ 358.botsalgn.Nested_358_botsalgn.omp2012.v1.0.tar.xz

#Args 2 and 3 are optional
#e.g.: source reconstruct_358_botsalgn.sh $SPEC
#The above cmd should be run in the destination directory and will use the default src_alt (358.botsalgn.Nested_358_botsalgn.omp2012.v1.0.tar.xz)

#Spec directory supplied must have standard location of benchmark src


SPEC=$1
DEST=${2:-.}
SRC_ALT=${3:-358.botsalgn.Nested_358_botsalgn.omp2012.v1.0.tar.xz}


ROOT=`pwd`

cd $DEST
DEST=`pwd`

cd $SPEC
source shrc

dumpsrcalt $DEST/$SRC_ALT > $DEST/$SRC_ALT.patch

cd benchspec/OMP2012/358.botsalgn/src
cp *.c $DEST
cp *.h $DEST
mkdir -p $DEST/omp-tasks/alignment/alignment_for
cp omp-tasks/alignment/alignment_for/*.c $DEST/omp-tasks/alignment/alignment_for
cp omp-tasks/alignment/alignment_for/*.h $DEST/omp-tasks/alignment/alignment_for
mkdir -p $DEST/common
cp common/*.c $DEST/common
cp common/*.h $DEST/common


cd $DEST

#patch --dry-run -ruN -p1 -d $DEST < $SRC_ALT.patch
patch -ruN -p1 -d $DEST < $SRC_ALT.patch

mv ./omp-tasks/alignment/alignment_for/alignment.c ./omp-tasks/alignment/alignment_for/alignment.cpp
mv ./common/bots_main.c ./common/bots_main.cpp
mv ./omp-tasks/alignment/alignment_for/sequence.c ./omp-tasks/alignment/alignment_for/sequence.cpp

ln -sv /home/dgerzhoy/Workspace/omp2012/benchspec/OMP2012/358.botsalgn/data data

cd $ROOT

