#!/bin/sh

file_input=$1
read_length=$2
undef_flag=0
maxError=10

error_threshold=1

# while [ $error_threshold -le $maxError ] 
# do
# 	echo ---------------------------------------------------------------------
# 	echo Execution begins with error_threshold = $error_threshold
# 	./edlib_run $file_input $undef_flag $read_length $error_threshold
# 	error_threshold=$((error_threshold+1))
# done
#
# for error_threshold in 2 5 7 10 12 15 17 20 22 25
for error_threshold in 1 3 4 6 7 9 10 12 13 15
do
 	echo ---------------------------------------------------------------------
	echo Execution begins with error_threshold = $error_threshold
	./edlib_run $file_input $undef_flag $read_length $error_threshold
 	echo ---------------------------------------------------------------------
done


echo PROGRAM COMPLETED
