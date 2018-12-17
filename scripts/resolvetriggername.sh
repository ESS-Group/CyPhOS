#!/bin/bash

MAPPING_FILE=$1
TRIGGER_ADDRESS=$(echo $2|sed 's/0x//')

echo "Checking address ${TRIGGER_ADDRESS} in mapping: ${MAPPING_FILE}"
grep "${TRIGGER_ADDRESS}" "${MAPPING_FILE}"
