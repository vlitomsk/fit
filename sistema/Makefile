all: clean compile run

clean: 
	rm -f ./main
	rm -f ./*.o

compile:
	gcc -Werror -c ./translator.c -o translator.o
	gcc -Werror -c ./main.c -o main.o
	gcc -Werror -c ./common.c -o common.o
	gcc ./*.o -o main -lm -Os

run:
	./main
