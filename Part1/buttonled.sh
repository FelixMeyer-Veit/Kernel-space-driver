#!/bin/bash
# Author: Felix Meyer-Veit

# Short Describtion of the script for the user
echo "Turn the LED on by pressing the button."
echo "Quit the shell script by pressing ctrl-c."

# export gpio pins
echo 17 > /sys/class/gpio/export
echo 27 > /sys/class/gpio/export
# wait for the creation of the two new directories
sleep 0.5
# set directions of gpio pins
echo "out" > /sys/class/gpio/gpio17/direction
echo "in" > /sys/class/gpio/gpio27/direction

# clean up when ctrl-c interrupt
trap '{ echo " --> buttonled.sh closed." ; echo 17 > /sys/class/gpio/unexport ; echo 27 > /sys/class/gpio/unexport ; exit 1; }' INT
# toggle Led according to the button value
COUNTER=0
while [ $COUNTER -lt 100000 ]; do
	button=$(cat /sys/class/gpio/gpio27/value)
	if [ $button = 1 ]
	then
		echo 1 > /sys/class/gpio/gpio17/value
	else
		echo 0 > /sys/class/gpio/gpio17/value
	fi
	let COUNTER=COUNTER+1
done

# unexport gpio pins
echo 17 > /sys/class/gpio/unexport
echo 27 > /sys/class/gpio/unexport
