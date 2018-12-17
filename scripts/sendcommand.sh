#!/bin/bash
DEVICE=$1
COMMAND=$2
echo -ne "\r${COMMAND}\r" > ${DEVICE}
