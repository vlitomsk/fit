all: clean compile run

clean: 
	rm -f ./main
	rm -f ./*.o

compile:
	gcc -c ./translator.c -o translator.o
	gcc -c ./main.c -o main.o
	gcc -c ./common.c -o common.o
	gcc ./*.o -o main -lm -Os

run:
	./main
