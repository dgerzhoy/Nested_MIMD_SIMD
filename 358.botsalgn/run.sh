#!/bin/bash -p

if [ -z "$4" ]
  then
    #echo "No unroll factor supplied"
	U=1
else
    #echo "Unroll factor supplied"
	U=$4
fi

time ./bots-alignment -f $1 -t $2 -k $3 -u $U 
