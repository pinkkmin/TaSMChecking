#!/bin/bash
for((i=1;i<=110;i++))
do
startTime=`date +%s%N`
./bitcount_sf 75000
endTime=`date +%s%N`
Time=`expr $endTime - $startTime`
rumTime=`expr $Time / 1000000`
echo $rumTime >>output/softboundcets.txt
# echo "run:$rumTime "
done