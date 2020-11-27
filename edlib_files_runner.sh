#!/bin/bash

# Modify only these:

# files="../../inputs/minimap2_samples/minimap2_ERR240727_1_E0_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E1_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E2_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E3_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E4_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E5_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E6_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E7_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E8_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E9_30million.seq
# ../../inputs/minimap2_samples/minimap2_ERR240727_1_E10_30million.seq"

files="../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E0_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E1_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E2_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E3_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E4_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E5_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E6_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E7_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E8_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E9_30million.seq
../../../inputs/bwa-mem_samples/extension/bwamem_ERR240727_1_E10_30million.seq"

file_name_="bwamem_ERR240727_1"
read_length=100
error_threshold=0
undef_flag=1
maxError=10

#DO NOT CHANGE ANYTHING AFTER THIS LINE:

for file in $files
do
	echo ---------------------------------------------------------------------
	echo "Processing $file"
	echo Execution begins with error_threshold = $error_threshold
	./edlib_run $file $undef_flag $read_length $error_threshold
 	echo ---------------------------------------------------------------------

	((error_threshold++))

done
echo ALL DONE ---------------------------------------------------------------------------



