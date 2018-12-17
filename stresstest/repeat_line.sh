#!/bin/bash
if [ $# -lt 2 ]
then
	echo "Usage $0 repeat_count input_string"
	exit
fi

REPEAT_COUNT=$1
INPUT_STRING="$2"

for i in $(seq 0 $REPEAT_COUNT)
do
	echo $INPUT_STRING|sed 's/__REPLACE__/'$i'/g'
done
