# rshell
spring 2015 rshell project

##Author

Jordan Kincer

##Overview

The program written into hw0.cpp has been written to perform many of the same tasks as the default shell that runs in a terminal.

A number of bash commands work in this shell (reffered from now on as rshell), such as:

	- ls
	- pwd
	- echo
	- cat
	- rm
	- mkdir
	- (and more...)
	
Just as well, a number of commands do NOT work in the rshell, such as:

	- cd

The rshell can also compile and run other programs (including itself).

##Files

README.md

LICENSE

Makefile

./src

./tests

###./src content

hw0.cpp

###./tests content

exec.script

##Known Bugs

Memory Leaks - When utilizing valgrind to record memory usage, a few memory allocations are never made free.

White space only - The program seg-faults whenever a command (whether on its own or between connectors) consists of ONLY whitespace.


