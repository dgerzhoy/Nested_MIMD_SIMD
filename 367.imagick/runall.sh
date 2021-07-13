#!/bin/bash

#usage: ./runall.sh

#GPU vs CPU is controlled by a CDefine for 367.imagick "-DOCL_FLEX"

export OMP_NUM_THREADS=1
INPUT=data/ref/input

make clean
make -j4

#CPU
touch magick_resize.cpp
touch utilities_convert.cpp
make

./convert convert.out convert.err -shear 31 -threads 1 -resize 12000x9000 $INPUT/input1.tga output1_1.tga |& tee Output_CPU1.txt
./convert convert.out convert.err -shear 31 -threads 4 -resize 12000x9000 $INPUT/input1.tga output1_4.tga |& tee Output_CPU4.txt

touch magick_resize.cpp
touch utilities_convert.cpp
make CFLAGS="-DOCL_FLEX"

./convert convert.out convert.err -shear 31 -threads 1 -resize 12000x9000 $INPUT/input1.tga output1_1.tga |& tee Output_FLEX1.txt
./convert convert.out convert.err -shear 31 -threads 4 -resize 12000x9000 $INPUT/input1.tga output1_4.tga |& tee Output_FLEX4.txt