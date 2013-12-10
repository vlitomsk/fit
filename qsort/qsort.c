#include "qsort.h"

void swap(int *a, int *b) {
	if (a != b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}

int *quicksort_lr(int *a, int l, int r) {
	int i = l, j = r;
	int p = a[(l + r) >> 1];
	do {
		while (a[i] < p) i++;
		while (a[j] > p) j--;
		//printf("%d %d %d\n", p, i, j);

		if (i <= j) {
			swap(a + i, a + j);
			++i, --j;
		}
	} while (i <= j);

	if (j > l)
		quicksort_lr(a, l, j);

	if (i < r)
		quicksort_lr(a, i, r);
	
	return a;
}

int* quicksort(int *a, const int N) {
	quicksort_lr(a, 0, N - 1);
}
