rel:
	mpic++ -DMPICH_IGNORE_CXX_SEEK -DMPICH_SKIP_MPICXX -I/usr/include/openmpi/ -march=native -mfpmath=sse -ftree-vectorizer-verbose=2 -O3 -std=c++11 -fdiagnostics-color=always DMatrix.cpp DVector.cpp Task.cpp Solver.cpp main.cpp -o main
	strip ./main

dbg:
	mpic++ -I/usr/include/openmpi/ -DDEBUG -g -O0 -std=c++11 -fdiagnostics-color=always debug.cpp DMatrix.cpp DVector.cpp Task.cpp Solver.cpp main.cpp -o main

gener:
	g++ -std=c++11 ./gen.cpp -o gen
	g++ -std=c++11 ./gensym.cpp -o gensym

clean:
	rm -f ./main
	rm -f ./gen ./gensym
