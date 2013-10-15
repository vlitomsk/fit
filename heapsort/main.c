#include <stdio.h>
#include <stdlib.h>
#include "heapsort.h"

int main() {
	int n, *arr, i, *res;
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	res = (int*)malloc(sizeof(int) * n);
	if (!arr || !res) {
		perror("Can't allocate bytes!\n");
		return 0;
	}

	for (i = 0; i < n; ++i)
		scanf("%d", &arr[i]);

	heapsort(arr, n);

	for (i = 0; i < n; ++i)
		printf("%d ", arr[i]);

	free(arr);
	free(res);
	return 0;
}

