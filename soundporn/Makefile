all: clean compile run

clean:
	rm ./*.o -f
	rm ./main -f

compile:
	gcc -c ./player.c -o player.o
	gcc -c ./wave.c -o wave.o
	gcc -c ./main.c -o main.o
	gcc ./*.o -o main -lm

run:
	./main | aplay

