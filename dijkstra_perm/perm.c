#include "perm.h"
#include <stdlib.h>
#include <string.h>

void swap(char *a, char *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

int cmp(const void *a, const void *b) {
	return *((const int*)a) - *((const int*)b);
}

void next_permutation(char *a) {
	int i, j, n = strlen(a), min, min_index;
	if (n == 1) {
		a[0] = 0;
		return;
	}

	for (i = n - 2; i >= 0; --i) 
		if (a[i] < a[i + 1])
			break;

	if (i == -1) {
		a[0] = 0;
		return;
	}
	
	for (j = i + 1, min = '9' + 2; j < n; ++j)
		if (a[j] < min && a[j] > a[i]) {
			min = a[j];
			min_index = j;
		}

	swap(&a[i], &a[min_index]);
	//printf("Swapping %d - %d\n", i, min_index);
	for (j = i + 1; j < (i + 1 + n) / 2; ++j) 
		swap(&a[j], &a[n - j + i]);
}
