#!/bin/bash
DEVICE=$1
COMMAND=$2
for (( i=0; i<${#COMMAND}; i++ )); do
  echo -ne "${COMMAND:$i:1}" > ${DEVICE}
  sleep 0.2
done
echo -ne "\r\r" > ${DEVICE}
#echo -ne "\r${COMMAND}\r" > ${DEVICE}
