#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

/****************************/

inline double calcPi(long long N) {
	long long i, M = 0;
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
#define MEASURE_COUNT 10

int main(int argc, char **argv) {
	long long N;
	int i;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <N> <out file>\n", argv[0]);
		exit(1);
	}
	freopen(argv[2], "w", stdout);
	sscanf(argv[1], "%lld", &N);

	struct tms start, end;

	double times_arr[MEASURE_COUNT];
	double min = 1e19;

	for (i = 0; i < MEASURE_COUNT; ++i) {
        times(&start);
		double PI = calcPi(N);
		times(&end);
		times_arr[i] = end.tms_utime - start.tms_utime;
		if (times_arr[i] < min)
		    min = times_arr[i];
		printf("Pi: %lf\nTime: %lf s\n\n", PI, times_arr[i]);
		fflush(stdout);
	}

	printf("==============\nMin. time: %lf quants\n", min);

	return 0;
}
