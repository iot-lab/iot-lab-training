#!/bin/bash
i=$1
for (( c=1; c<=i; c++ ))
do
 ./start-one.sh $1 & 
done
