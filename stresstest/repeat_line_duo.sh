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
	# Set connection between components, component i connects to i+1
	NEXT_COMPONENT=$i
	NEXT_COMPONENT=$((NEXT_COMPONENT+1))

	if [ $i -eq $REPEAT_COUNT ]
	then
		# Unless it is the last one, then it connects to component 0
		NEXT_COMPONENT=0
	fi


	echo $INPUT_STRING|sed 's/__REPLACE__/'$i'/g; s/__NEXT__/'$NEXT_COMPONENT'/g'
done
