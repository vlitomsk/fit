all: compile run

#CC = gcc
#CC = i686-w64-mingw32-gcc
CC = gcc -O2 

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile: clean
	$(CC) ./main.c -o main.o
	$(CC) ./*.o -o main.exe -lm

run: clean compile
	./main.exe < input 
	
test: compile
	./testLab4.exe main.exe
