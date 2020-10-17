# Kernel-space-driver
Project 4 in EMBE with Erik Leffler

For this project we used a Raspberry Pi single board computer and components of choice.
Wire up an input and an output, e.g. using a button and an LED.
Compare the response time of three implementations. 

Part 1
Using sysfs from the shell (command line).   Write a shell script that makes the button operate the LED.
Measure the response time.

Part 2
Implement the same functionality with a user-space C/C++ application, that communicates with a kernel space driver using sysfs.

Part3
Implement the button response within kernel space, by using interrupts.
