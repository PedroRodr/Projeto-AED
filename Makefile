CC=gcc
CFLAGS=-ansi -pedantic -Wall -g

sepa: projfinal.o 
	gcc -o sepa projfinal.o

clean:
	rm -f *.o sepa

# DO NOT DELETE THIS LINE -- make depend depends on it.
