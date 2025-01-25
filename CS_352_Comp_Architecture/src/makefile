CC = gcc #

default: main

main: main.o func.o
	$(CC) -g -Wall -o main main.o func.o

main.o: main.c func.h
	$(CC) -g -Wall -c main.c
	
func.o: func.c func.h
	$(CC) -g -Wall -c func.c

clean:
	$(RM) main.exe *.o *.~ *.out
