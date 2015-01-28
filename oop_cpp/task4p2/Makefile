all: clean
	g++ -std=c++11 ./*.cpp -o main `pkg-config --cflags --libs tclap`

clean:
	rm -f ./main
