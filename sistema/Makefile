all: clean compile run

clean: 
	rm -f ./main
	rm -f ./*.o

compile:
	gcc -c -O3 ./translator.c -o translator.o
	gcc -c -O3 ./main.c -o main.o
	gcc -c -O3 ./common.c -o common.o
	gcc ./*.o -o main -lm -O3

run:
	./main
