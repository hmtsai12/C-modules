all:hello.o
	gcc -finstrument-functions hello.o -o hello
hello.o:
	gcc -finstrument-functions -finstrument-functions-exclude-function-list=test -c hello.c
clean:
	rm *.o hello -rf
