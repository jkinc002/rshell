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

##Running rshell

Perform these commands to download and run the program:
```
	$ git clone https://github.com/jkinc002/rshell
	$ cd rshell
	$ git checkout hw0
	$ make
	$ bin/rshell
```

##Known Bugs

Memory Leaks - When utilizing valgrind to record memory usage, a few memory allocations are shown to have never been made free.

White space only - The program seg-faults whenever a command (whether on its own or between connectors) consists of ONLY whitespace.

Username display - Running rshell after running 'script exec.script' causes the username to not be displayed properly. The name displays normally again after ending the script session.

Argument limit - The program seg-faults if a single argument (not including neighboring arguments) contains +1024 words (characters separated by spaces).


