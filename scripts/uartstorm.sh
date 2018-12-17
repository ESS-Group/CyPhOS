#!/bin/bash
UART_DEVICE=$1
SEND_CHAR=$2
SLEEP_TIME=$3
ITERATION_COUNT=$4

for i in $(seq 1 ${ITERATION_COUNT})
do
	echo "Uart write: ${i}"
	echo -n ${SEND_CHAR} > ${UART_DEVICE}
	sleep ${SLEEP_TIME}
done
