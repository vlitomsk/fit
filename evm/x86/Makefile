all: clean
	gcc -masm=intel -m32 -O2 -S ./monte.c -o monte.s 
	gcc -O0 --static -g ./monte.c -o monte -lrt

clean:
	rm -f monte.s
	rm -f monte

