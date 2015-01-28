#!/bin/bash
rm -f main
rm -f *.o
g++ -g -std=c++11 -c ./HashTable.cpp -o HashTable.o -O0
g++ -g -std=c++11 -c ./main.cpp -o main.o -O0
g++ -g *.o -std=c++11 -o main -O0 -lm 
rm -f *.o
