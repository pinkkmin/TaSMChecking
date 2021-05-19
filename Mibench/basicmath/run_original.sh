#!/bin/bash
for((i=1;i<100;i++))
do
startTime=`date +%s%N`
./basicmath_or input_small.dat
endTime=`date +%s%N`
Time=`expr $endTime - $startTime`
rumTime=`expr $Time / 1000000`
echo $rumTime >>output/original.txt
done