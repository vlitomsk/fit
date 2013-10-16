all: compile run

#CC = gcc
#CC = i686-w64-mingw32-gcc
CC = gcc

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile: clean
	$(CC) -c -O2 ./heapsort.c -o heapsort.o
	$(CC) -c -O2 ./main.c -o main.o
	$(CC) -O2 ./*.o -o main.exe -lm

run: clean compile
	./main.exe < input 
	
test: compile
	./testLab30.exe main.exe
