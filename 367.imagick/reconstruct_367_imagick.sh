#!/bin/bash

#AUTO-GENERATED SCRIPT TO RECONSTRUCT 367.imagick

#Usage: source reconstruct_367_imagick.sh <spec root> <spec benchmark dir> <destination dir> <src_alt>
#e.g.: source reconstruct_367_imagick.sh $SPEC ./ 367.imagick.Nested_367_imagick.omp2012.v1.0.tar.xz

#Args 2 and 3 are optional
#e.g.: source reconstruct_367_imagick.sh $SPEC
#The above cmd should be run in the destination directory and will use the default src_alt (367.imagick.Nested_367_imagick.omp2012.v1.0.tar.xz)

#Spec directory supplied must have standard location of benchmark src


SPEC=$1
DEST=${2:-.}
SRC_ALT=${3:-367.imagick.Nested_367_imagick.omp2012.v1.0.tar.xz}


ROOT=`pwd`

cd $DEST
DEST=`pwd`

cd $SPEC
source shrc

dumpsrcalt $DEST/$SRC_ALT > $DEST/$SRC_ALT.patch

cd benchspec/OMP2012/367.imagick/src
cp *.c $DEST
cp *.h $DEST
mkdir $DEST/wand
cp wand/*.c $DEST/wand
cp wand/*.h $DEST/wand
mkdir $DEST/magick
cp magick/*.c $DEST/magick
cp magick/*.h $DEST/magick


cd $DEST

#patch --dry-run -ruN -p1 -d $DEST < $SRC_ALT.patch
patch -ruN -p1 -d $DEST < $SRC_ALT.patch

mv utilities_convert.c utilities_convert.cpp
mv magick_resize.c magick_resize.cpp

ln -sv /home/dgerzhoy/Workspace/omp2012/benchspec/OMP2012/367.imagick/data data

cd $ROOT

