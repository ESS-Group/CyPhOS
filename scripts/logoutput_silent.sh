#!/bin/bash
DEVICE=$1
OUTPUT_PREFIX="$2"

cat ${DEVICE} > "${OUTPUT_PREFIX}"_$(date +%d-%m-%y-%H-%M).log
