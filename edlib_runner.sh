#!/bin/sh

file_input=$1
read_length=$2
undef_flag=1
maxError=15

error_threshold=0

#while [ $error_threshold -le $maxError ] 
#do
#	echo ---------------------------------------------------------------------
#	echo Execution begins with error_threshold = $error_threshold
#	./edlib_run $file_input $undef_flag $read_length $error_threshold
#	error_threshold=$((error_threshold+1))
#done

for error_threshold in 0 1 2 3 4 6 8 10 15
do
 	echo ---------------------------------------------------------------------
	echo Execution begins with error_threshold = $error_threshold
	./edlib_run $file_input $undef_flag $read_length $error_threshold
 	echo ---------------------------------------------------------------------
done


echo PROGRAM COMPLETED