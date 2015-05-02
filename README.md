# cs100 homework projects

#Contents
	- ls
	- rshell

##Author
Jordan Kincer

##Files

README.md

LICENSE

Makefile

./src

./tests

###./src content

hw0.cpp

hw1.cpp

ls.h

###./tests content

exec.script

ls.script

# ls

##Overview

This program aims to recreate the bash command ls.

This ls supports the flags:
	- -l
	- -a

In the future the -R flag will be implemented correctly.

The ls also supports pathnames that are passed in as arguments, and displays their contents appropriately.

##Running ls

Perform these commands to download and run the program:
```
	$ git clone https://github.com/jkinc002/rshell
	$ cd rshell
	$ git checkout hw1
	$ make
	$ bin/ls
```

##Known Bugs - ls

R flag - Though intended to work, flagging R will cause the program to seg-fault. The problem is caused by a pointer being deallocated before it can be used.

# rshell
spring 2015 rshell project

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


##Running rshell

Perform these commands to download and run the program:
```
	$ git clone https://github.com/jkinc002/rshell
	$ cd rshell
	$ git checkout hw0
	$ make
	$ bin/rshell
```

##Known Bugs - rshell

White space only - The program seg-faults whenever a command (whether on its own or between connectors) consists of ONLY whitespace.

Username display - Running rshell after running 'script exec.script' causes the username to not be displayed properly. The name displays normally again after ending the script session.

Argument limit - The program seg-faults if a single argument (not including neighboring arguments) contains +1024 words (characters separated by spaces).

Connector symbol - Mistyping `&&` as `&`, or `||` as `|`, will cause the connector to behave as a `;` connector.

Sequential connectors - An input such as `$ ls &&;;;;||||&&;; ls` will behave only according to the very first connector in the sequence. ('&&' in this case).


