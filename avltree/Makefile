all: compile run

CC = gcc
#CC = i686-w64-mingw32-gcc
#CC = i586-mingw32msvc-gcc

clean:
	del *.o
	del main.exe

compile: clean
	$(CC) -g -c -O2 ./avltree.c -o avltree.o
	$(CC) -g -c -O2 ./main.c -o main.o
	$(CC) -g -O2 ./*.o -o main.exe -lm

run: clean compile
	main.exe < in.txt
	
test: compile
	tester.exe main.exe
