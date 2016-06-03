#!/bin/sh

if ! [ -x ./log ]; then
	mkdir log
fi

for nbuffers in 1 2 4; do
	for thread_count in 1 2 3 4 5 6 7 8; do
		make clean
		make main NBUFFERS=$nbuffers THREAD_COUNT=$thread_count
		./main > ./log/${nbuffers}_buf_${thread_count}_threads
	done
done
