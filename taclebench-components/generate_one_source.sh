#!/bin/bash -x

BENCHMARKS="filterbank fir2dim iir insertsort jfdctint lms ludcmp matrix1 md5 minver pm prime quicksort recursion sha st"

BASE_BENCHMARK="binarysearch"

for i in $BENCHMARKS
do
	echo "Doing benchmark: $i"
	mkdir $i
	# Copy base files
	cp $BASE_BENCHMARK/* $i/

	# Modify module.mk
	UPPERCASE_NAME=${i^^}

	UPPERCASE_BASE=${BASE_BENCHMARK^^}

	pushd $i
	sed -i 's/'$UPPERCASE_BASE'/'$UPPERCASE_NAME'/g; s/'$BASE_BENCHMARK'/'$i'/g' module.mk
	
	mv ${BASE_BENCHMARK}.cc ${i}.cc
	sed -i 's/'$UPPERCASE_BASE'/'$UPPERCASE_NAME'/g; s/'$BASE_BENCHMARK'/'$i'/g' ${i}.cc
	mv ${BASE_BENCHMARK}.h ${i}.h
	sed -i 's/'$UPPERCASE_BASE'/'$UPPERCASE_NAME'/g; s/'$BASE_BENCHMARK'/'$i'/g' ${i}.h

	mv Tacle${BASE_BENCHMARK}Component.cc Tacle${i}Component.cc
	sed -i 's/'$UPPERCASE_BASE'/'$UPPERCASE_NAME'/g; s/'$BASE_BENCHMARK'/'$i'/g' Tacle${i}Component.cc
	mv Tacle${BASE_BENCHMARK}Component.h Tacle${i}Component.h
	sed -i 's/'$UPPERCASE_BASE'/'$UPPERCASE_NAME'/g; s/'$BASE_BENCHMARK'/'$i'/g' Tacle${i}Component.h
	popd
done
