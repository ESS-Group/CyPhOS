#!/bin/bash
DEVICE=$1
OUTPUT_PREFIX="$2"

cat ${DEVICE} | tee "${OUTPUT_PREFIX}"_$(date +%H-%M_%d-%m-%y)
