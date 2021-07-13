#!/bin/bash

#AUTO-GENERATED SCRIPT TO RECONSTRUCT 372.smithwa

#Usage: source reconstruct_372_smithwa.sh <spec root> <spec benchmark dir> <destination dir> <src_alt>
#e.g.: source reconstruct_372_smithwa.sh $SPEC ./ 372.smithwa.Nested_372_smithwa.omp2012.v1.0.tar.xz

#Args 2 and 3 are optional
#e.g.: source reconstruct_372_smithwa.sh $SPEC
#The above cmd should be run in the destination directory and will use the default src_alt (372.smithwa.Nested_372_smithwa.omp2012.v1.0.tar.xz)

#Spec directory supplied must have standard location of benchmark src


SPEC=$1
DEST=${2:-.}
SRC_ALT=${3:-372.smithwa.Nested_372_smithwa.omp2012.v1.0.tar.xz}


ROOT=`pwd`

cd $DEST
DEST=`pwd`

cd $SPEC
source shrc

dumpsrcalt $DEST/$SRC_ALT > $DEST/$SRC_ALT.patch

cp benchspec/OMP2012/372.smithwa/src/*.c $DEST
cp benchspec/OMP2012/372.smithwa/src/*.h $DEST

cd $DEST

patch -ruN -d $DEST < $SRC_ALT.patch

mv sequenceAlignment.c sequenceAlignment.cpp
mv pairwiseAlign.c pairwiseAlign.cpp

cd $ROOT

