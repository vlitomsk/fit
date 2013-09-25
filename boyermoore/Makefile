all: clean compile run

clean:
	rm -f ./*.o
	rm -f ./main.exe

compile:
	gcc -c -O3 ./bmoore.c -o bmoore.o
	gcc -c -O3 ./main.c -o main.o
	gcc ./*.o -o main.exe -lm

run:
	./main.exe < input
	
