#!/bin/bash
# Author: Felix Meyer-Veit
# just a test
# Short Describtion of the script for the user
echo "LED is toggled."
echo "Quit the shell script by pressing ctrl-c."

# export gpio pins
echo 17 > /sys/class/gpio/export
# wait for the creation of the two new directories
sleep 0.5
# set directions of gpio pins
echo "out" > /sys/class/gpio/gpio17/direction

# clean up when ctrl-c interrupt
trap '{ echo " --> toggle.sh closed." ; echo 17 > /sys/class/gpio/unexport ; exit 1; }' INT
# toggle Led
COUNTER=0
while [ $COUNTER -lt 100000000000000000 ]; do
	echo 1 > /sys/class/gpio/gpio17/value
	sleep 1
	echo 0 > /sys/class/gpio/gpio17/value
	sleep 1
	let COUNTER=COUNTER+1
done

# unexport gpio pins
echo 17 > /sys/class/gpio/unexport
