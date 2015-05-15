CFLAGS = -Wall -Werror -ansi -pedantic -g

all: bin rshell ls cp

bin:
	mkdir bin

rshell: src/hw2.cpp src/myio.h
	g++ $(CFLAGS) src/hw2.cpp -o ./bin/rshell

ls:
	g++ $(CFLAGS) src/hw1.cpp -o ./bin/ls

cp: src/cp.cpp src/Timer.h
	g++ $(CFLAGS) src/cp.cpp -o ./bin/cp

clean:
	rm -f *.0 rshell
	rm -f *.0 ls
	rm -rf *.0 bin
	echo CLEAN DONE
	
