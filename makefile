all: makeproject
makeproject: main.o
	gcc main.o -o makeproject -lsense -lm
makeproject.o: main.c
	gcc -c main.c
clean:
	rm -f *.o makeproject
