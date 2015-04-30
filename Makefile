CFLAGS = -Wall -Werror -ansi -pedantic

all: bin rshell cp

bin:
	mkdir bin

rshell: src/hw0.cpp
	g++ $(CFLAGS) src/hw0.cpp -o ./bin/rshell

cp: src/cp.cpp src/Timer.h
	g++ $(CFLAGS) src/cp.cpp -o ./bin/cp

clean:
	rm -f *.0 rshell
	rm -rf *.0 bin
	echo CLEAN DONE
	
