all: compile run

#CC = gcc
CC = i686-w64-mingw32-gcc -O2
#CC = gcc -O2 -g

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile: clean
	$(CC) -c ./main.c -o main.o
	$(CC) -c ./calc.c -o calc.o
	$(CC) -c ./calctypes.c -o calctypes.o
	$(CC) ./*.o -o main.exe -lm

run: clean compile
	./main.exe < in.txt
	
test: compile
	./testLab4.exe main.exe
