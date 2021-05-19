#!/bin/bash
startTime=`date +"%Y-%m-%d %H:%M:%S"`

./qsort_small input_small.dat
endTime=`date +"%Y-%m-%d %H:%M:%S"`
st=`date -d  "$startTime" +%s`
et=`date -d  "$endTime" +%s`
sumTime=$(($et-$st))
echo "Total time is : $sumTime second."