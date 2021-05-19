#!/bin/bash
for((i=1;i<110;i++))
do
startTime=`date +%s%N`
./fft_or 4 4096
endTime=`date +%s%N`
Time=`expr $endTime - $startTime`
rumTime=`expr $Time / 1000000`
echo $rumTime >>output/original.txt
done