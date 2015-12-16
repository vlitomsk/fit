all:
	mpicxx main2.cpp -std=c++11 -o main -Wextra -Werror -O2
	strip ./main

de:
	mpicxx main2.cpp -std=c++11 -o main -Wextra -Werror -DDEBUG -g -O0

clean:
	rm -f main

