#!/bin/bash

if [ $# -lt 2 ]
then
	echo Usage "$0" component_count template_name
	exit
fi

COMPONENT_COUNT=$1
TEMPLATE_NAME=$2

CC_FILES=""

for i in $(seq 0 $COMPONENT_COUNT)
do
	# Set connection between components, component i connects to i+1
	NEXT_COMPONENT=$i
	NEXT_COMPONENT=$((NEXT_COMPONENT+1))
	if [ $i -eq $COMPONENT_COUNT ]
	then
		# Unless it is the last one, then it connects to component 0
		NEXT_COMPONENT=0
	fi

	# Determine destination source file name
	SOURCE_FILE="$(echo $2|sed 's/draft/'$i'/g').cc"
	HEADER_FILE="$(echo $2|sed 's/draft/'$i'/g').h"

	CC_FILES="$CC_FILES $SOURCE_FILE"
	echo Generating component "$i" connected to ${NEXT_COMPONENT} with source file $SOURCE_FILE
	INPUT_SOURCE="$TEMPLATE_NAME.cc"
	INPUT_HEADER="$TEMPLATE_NAME.h"
	# Generate source file
	sed 's/__THISCOMPONENT__/'$i'/g; s/__NEXTCOMPONENT__/'${NEXT_COMPONENT}'/g' $INPUT_SOURCE > $SOURCE_FILE
	# Generate header file
	sed 's/__THISCOMPONENT__/'$i'/g; s/__NEXTCOMPONENT__/'${NEXT_COMPONENT}'/g' $INPUT_HEADER > $HEADER_FILE

done

echo "Source files: $CC_FILES"
