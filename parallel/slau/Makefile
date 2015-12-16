all: gen
	mpicc -O2 -Wextra -fdiagnostics-color=always *.c -o parlab
	mpirun -np 2 ./parlab ./mat

gen:
	g++ ./genmat.cpp -o gen

debug: gen
	mpicc -g -O0 -Wextra -fdiagnostics-color=always *.c -o parlab

clean:
	rm -f ./gen ./mat ./parlab
