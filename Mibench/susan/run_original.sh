#!/bin/bash
for((i=1;i<110;i++))
do
startTime=`date +%s%N`
./susan_or input_small.pgm output_large.smoothing.pgm -s
endTime=`date +%s%N`
Time=`expr $endTime - $startTime`
rumTime=`expr $Time / 1000000`
echo $rumTime >>output/original.txt
done