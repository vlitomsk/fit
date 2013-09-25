all: clean compile run

#CC = gcc
CC = i686-w64-mingw32-gcc

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile:
	$(CC) -c -O3 ./bmoore.c -o bmoore.o
	$(CC) -c -O3 ./main.c -o main.o
	$(CC) ./*.o -o main.exe -lm

run:
	./main.exe < input
	
