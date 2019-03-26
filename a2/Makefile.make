all: anthills 

anthills: aardvarks.o anthills.o
	gcc -g -o anthills aardvarks.o anthills.o -lpthread -lrt
aardvarks.o: aardvarks.c anthills.h
	gcc -g -c aardvarks.c
anthills.o: anthills.c anthills.h
	gcc -g -c anthills.c
