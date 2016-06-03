SPUP=spu_prog
PPUP=ppu_prog
SYMB=$(shell cat symbol)
LABNAME=$(shell basename "`pwd`")
TIMESTAMP=$(shell date +%s)
NBUFFERS=1
THREAD_COUNT=5
CFLAGS=-DSPU_SYMB=$(SYMB) -DNBUFFERS=$(NBUFFERS) -DTHREAD_COUNT=$(THREAD_COUNT)
CELLWDIR=nsulab@ssd1.sscc.ru:~/litvinov
CELLABPATH=$(CELLWDIR)/$(LABNAME)

all: main

run: main
	./main

main: $(SPUP).o $(PPUP).o
	ppu-gcc $(CFLAGS) -o main $(PPUP).o $(SPUP).o -lspe2 -lrt -lm

$(PPUP).o: $(PPUP).c consts.h
	ppu-gcc $(CFLAGS) -c -o $(PPUP).o $(PPUP).c

$(SPUP).o: $(SPUP)
	ppu-embedspu $(SYMB) $(SPUP) $(SPUP).o

$(SPUP): $(SPUP).c consts.h
	spu-gcc $(CFLAGS) -I/usr/lib/gcc/spu/4.3.2/include/ -o $(SPUP)  $(SPUP).c

clean:
	rm -f *.o $(SPUP) main

push: clean backup
	scp -r -P 2294 ./* $(CELLABPATH)/

pull: backup
	scp -r -P 2294 $(CELLABPATH)/* ./

backup: clean
	cd .. && tar cjvf bkup_$(LABNAME)_$(TIMESTAMP).tar.bz2 $(LABNAME)

