#!/bin/bash
i=$1
for (( c=1; c<=i; c++ ))
do
 ./script_saclay.sh $1 & 
done