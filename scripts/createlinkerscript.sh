#!/bin/bash

OBJECT_DUMP=$3

OBJECT_FILE_DIR=$1

OUTPUT_FILE=$2

ALIGNMENT=$4

# Iterate over all object files and create a list of OSC namespaces and their object files
OBJECT_FILES=$(find ${OBJECT_FILE_DIR} -type f -name *.o)

# Assosiative array for Namespace->Object files mapping
declare -A OBJECT_FILES_PER_NS

LINKER_SCRIPT=""

for FILE in $OBJECT_FILES
do
	echo "Searching in object file $FILE"
	OSC_NAMESPACES=$(${OBJECT_DUMP} --demangle -t -j .text -j .bss $FILE|egrep -e "\.text" -e "\.bss" -e "\.data"|egrep "NS_OSC_[[:alnum:]_]+_OSC" -o)

	OSC_TEMP_NAMESPACES=""
	# Validate that only one OSC namespace is used per object file
	for OSC_NS in ${OSC_NAMESPACES}
	do
		# Check if current OSC ns was found already
		if [ $(echo $OSC_TEMP_NAMESPACES|grep "$OSC_NS\$"|wc -l) -eq 0 ]
		then
			# If not check if any other was found already and abort if multiple NS_OSCs were found
			if [ "$OSC_TEMP_NAMESPACES" == "" ]
			then
				echo "Found OSC namespace $OSC_NS"
				OSC_TEMP_NAMESPACES="$OSC_NS"
			else
				echo "Multiple OSC namespaces (${OSC_TEMP_NAMESPACES}) and ${OSC_NS} in object file (${FILE}), abort!"
				exit
			fi
		else
			echo "Found OSC namespace $OSC_NS but it was already found"
		fi
	done
	if [ "$OSC_TEMP_NAMESPACES" != "" ]
	then
		OBJECT_FILES_PER_NS["$OSC_TEMP_NAMESPACES"]="${OBJECT_FILES_PER_NS["$OSC_TEMP_NAMESPACES"]} $FILE"
	fi
done

# iterate over all found OSC namespaces
for NS in ${!OBJECT_FILES_PER_NS[@]}
do
	NAMESPACE=$NS
	echo $NAMESPACE":"
	# Align every OSC at 64KB boundaries
	LINKER_SCRIPT="${LINKER_SCRIPT}\n. = ALIGN(${ALIGNMENT});\n.${NAMESPACE} :\n{"
	LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_start = .;"
	for objectfile in ${OBJECT_FILES_PER_NS["$NS"]}
	do
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_text_start = .;"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    ${objectfile}(\".text*\")"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_text_end = .;"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    . = ALIGN(0x4);"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    ${objectfile}(\".data*\")"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_bss_start = .;"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    ${objectfile}(\".bss\")"
		LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_bss_end = .;"
	done
	LINKER_SCRIPT="${LINKER_SCRIPT}\n    __${NS}_end = .;"
#	LINKER_SCRIPT="${LINKER_SCRIPT}\n    ASSERT( . - __${NS}_start <= ${ALIGNMENT} , \"OSC_${NS} >64k\");"
	LINKER_SCRIPT="${LINKER_SCRIPT}\n}"


done

echo -e ${LINKER_SCRIPT} > ${OUTPUT_FILE}

