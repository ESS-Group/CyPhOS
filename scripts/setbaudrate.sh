#!/bin/bash
DEVICE=$1
BAUDRATE=$2
stty -F ${DEVICE} ${BAUDRATE} cs8 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke  
