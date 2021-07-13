#!/bin/bash

#AUTO-GENERATED SCRIPT TO RECONSTRUCT 330.art_m

#Usage: source reconstruct_330_art_m.sh <spec root> <spec2001 330_art_m dir> <destination dir> <src_alt>
#e.g.: source reconstruct_330_art_m.sh $SPEC $SPEC2001/330.art_m ./ 330.art_m.Nested_330_art_m.omp2012.v1.0.tar.xz

#Args 2 and 3 are optional
#e.g.: source reconstruct_330_art_m.sh $SPEC
#The above cmd should be run in the destination directory and will use the default src_alt (330.art_m.Nested_330_art_m.omp2012.v1.0.tar.xz)

#Spec directory supplied must have standard location of benchmark src


SPEC=$1
SPEC2001=$2
DEST=${3:-.}
SRC_ALT=${4:-330.art_m.Nested_330_art_m.omp2012.v1.0.tar.xz}


ROOT=`pwd`

cd $DEST
DEST=`pwd`

cd $SPEC
source shrc

mkdir -p benchspec/OMP2012/330.art_m/src
cp $SPEC2001/scanner.c benchspec/OMP2012/330.art_m/src

dumpsrcalt $DEST/$SRC_ALT > $DEST/$SRC_ALT.patch

cd benchspec/OMP2012/330.art_m/src
cp *.c $DEST
cp *.h $DEST


cd $DEST

#patch --dry-run -ruN -p1 -d $DEST < $SRC_ALT.patch
patch -ruN -p1 -d $DEST < $SRC_ALT.patch

mv ./scanner.c ./scanner.cpp

cd $ROOT

