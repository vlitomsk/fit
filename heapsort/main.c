#include <stdio.h>
#include <stdlib.h>
#include "heapsort.h"

int main() {
	int n, *arr, i;
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	if (!arr) {
		perror("Can't allocate bytes!\n");
		return 0;
	}

	for (i = 0; i < n; ++i)
		scanf("%d", &arr[i]);

	heapsort(arr, n);

	for (i = 0; i < n; ++i)
		printf("%d ", arr[i]);

	free(arr);
	return 0;
}

