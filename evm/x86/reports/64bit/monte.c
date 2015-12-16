#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/****************************/

inline double calcPi(unsigned N) {
	unsigned i, M = 0;
	double x, y;
	for (i = 0; i < N; ++i) { 
		x = (double)rand() * (2.0 / (double)RAND_MAX) - 1.0;
		y = (double)rand() * (2.0 / (double)RAND_MAX) - 1.0;
		if (x * x + y * y < 1.0) 
			M++;
	}

	return 4.0 * (double)M / (double)N;
}

/**************************/

int main(int argc, char **argv) {
	unsigned N;
	int i;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <N> <out file>\n", argv[0]);
		exit(1);
	}
	freopen(argv[2], "w", stdout);
	sscanf(argv[1], "%u", &N);

	struct timespec start, end;
	clock_getres(CLOCK_MONOTONIC_RAW, &start);
	printf("Resolution: %d ns\n\n", start.tv_nsec);
	fflush(stdout);
	double time;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    double PI = calcPi(N);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    time = end.tv_sec - start.tv_sec + 1e-9*(end.tv_nsec - start.tv_nsec);
    printf("Pi: %lf\nTime: %lf s\n\n", PI, time);

	return 0;
}
