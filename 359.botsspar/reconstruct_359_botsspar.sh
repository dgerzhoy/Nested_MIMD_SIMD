#!/bin/bash

#AUTO-GENERATED SCRIPT TO RECONSTRUCT 359.botsspar

#Usage: source reconstruct_359_botsspar.sh <spec root> <spec benchmark dir> <destination dir> <src_alt>
#e.g.: source reconstruct_359_botsspar.sh $SPEC ./ 359.botsspar.Nested_359_botsspar.omp2012.v1.0.tar.xz

#Args 2 and 3 are optional
#e.g.: source reconstruct_359_botsspar.sh $SPEC
#The above cmd should be run in the destination directory and will use the default src_alt (359.botsspar.Nested_359_botsspar.omp2012.v1.0.tar.xz)

#Spec directory supplied must have standard location of benchmark src


SPEC=$1
DEST=${2:-.}
SRC_ALT=${3:-359.botsspar.Nested_359_botsspar.omp2012.v1.0.tar.xz}


ROOT=`pwd`

cd $DEST
DEST=`pwd`

cd $SPEC
source shrc

dumpsrcalt $DEST/$SRC_ALT > $DEST/$SRC_ALT.patch

cd benchspec/OMP2012/359.botsspar/src
cp *.c $DEST
cp *.h $DEST
mkdir -p $DEST/common
cp common/*.c $DEST/common
cp common/*.h $DEST/common
mkdir -p $DEST/omp-tasks/sparselu/sparselu_single
cp omp-tasks/sparselu/sparselu_single/*.c $DEST/omp-tasks/sparselu/sparselu_single
cp omp-tasks/sparselu/sparselu_single/*.h $DEST/omp-tasks/sparselu/sparselu_single


cd $DEST

#patch --dry-run -ruN -p1 -d $DEST < $SRC_ALT.patch
patch -ruN -p1 -d $DEST < $SRC_ALT.patch

mv ./common/bots_main.c ./common/bots_main.cpp
mv ./omp-tasks/sparselu/sparselu_single/sparselu.c ./omp-tasks/sparselu/sparselu_single/sparselu.cpp

ln -sv /home/dgerzhoy/Workspace/omp2012/benchspec/OMP2012/359.botsspar/data data

cd $ROOT

