all: clean
	arm-linux-gnueabi-gcc -O2 -S ./monte.c -o monte.s 
	arm-linux-gnueabi-gcc -O0 --static -g ./monte.c -o monte -lrt

clean:
	rm -f monte.s
	rm -f monte

