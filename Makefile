CFLAGS = -Wall -Werror -ansi -pedantic

all: bin rshell

bin:
	mkdir bin

rshell:
	g++ $(CFLAGS) src/hw0.cpp -o ./bin/rshell

clean:
	rm -f *.0 rshell
	rm -rf *.0 bin
	echo CLEAN DONE
	
