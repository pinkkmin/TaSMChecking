#!/bin/bash
for((i=1;i<110;i++))
do
startTime=`date +%s%N`
./crc_or ../adpcm/data/small.pcm 
endTime=`date +%s%N`
Time=`expr $endTime - $startTime`
rumTime=`expr $Time / 1000000`
echo $rumTime >>output/original.txt
done