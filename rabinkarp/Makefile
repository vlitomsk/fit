all: compile run

#CC = gcc
CC = i686-w64-mingw32-gcc
#CC = gcc

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile: clean
	$(CC) -c -O3 ./rkarp.c -o rkarp.o
	$(CC) -c -O3 ./main.c -o main.o
	$(CC) ./*.o -o main.exe -lm

run: clean compile
	wine ./main.exe < input 
	
test: compile
	wine ./testLab11.exe main.exe
