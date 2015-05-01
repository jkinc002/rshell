CFLAGS = -Wall -Werror -ansi -pedantic -g

all: bin rshell ls

bin:
	mkdir bin

rshell:
	g++ $(CFLAGS) src/hw0.cpp -o ./bin/rshell

ls:
	g++ $(CFLAGS) src/hw1.cpp -o ./bin/ls

clean:
	rm -f *.0 rshell
	rm -f *.0 ls
	rm -rf *.0 bin
	echo CLEAN DONE
	
