# Olympia Axelou 2161

all: partA.o partB.o synchron_functions.o synchron_functions.h
	gcc partA.o synchron_functions.o -o partA
	gcc partB.o synchron_functions.o -o partB

partA: partA.o synchron_functions.o synchron_functions.h
	gcc partA.o synchron_functions.o -o partA

partB: partB.o synchron_functions.o synchron_functions.h
	gcc partB.o synchron_functions.o -o partB

partA.o: partA.c synchron_functions.h
	gcc -Wall -c partA.c

partB.o: partB.c synchron_functions.h
	gcc -Wall -c partB.c

synchron_functions.o: synchron_functions.c synchron_functions.h
	gcc -Wall -c synchron_functions.c