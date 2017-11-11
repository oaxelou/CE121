# Olympia Axelou 2161
all: hw3.o lists_functions.o lists_functions.h
	gcc hw3.o lists_functions.o -o hw3
	gcc -Wall -g test1.c -o test1
	gcc -Wall -g test2.c -o test2
	gcc -Wall -g test3.c -o test3
	
hw3.o: hw3.c lists_functions.h
	gcc -Wall -c hw3.c

lists_functions.o: lists_functions.c lists_functions.h
	gcc -Wall -c lists_functions.c

.PHONY: clean
clean: hw3 test1 test2 test3
	rm *.o hw3 test1 test2 test3