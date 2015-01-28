#!/bin/bash
g++ -g --coverage -pipe  -std=c++11 -c ./HashTable.cpp -o HashTable.o -O0 
g++ -g --coverage -pipe  -std=c++11 -c ./test.cpp -o test.o -O0 -I/home/vas/local/include
g++ -g --coverage -pipe  *.o -std=c++11 -o test -O0 -lm -L/home/vas/local/lib -lpthread -lgtest -lgtest_main
rm -f *.o
