#include <stdlib.h>
#include "qsort.h"

int main() {
	int n, *arr, i;
	scanf("%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	if (!arr) {
		perror("Can't allocate memory!\n");
		return 1;
	}
	
	for (i = 0; i < n; ++i)
		scanf("%d", arr + i);
	quicksort(arr, n);
	for (i = 0; i < n; ++i) 
		printf("%d ", arr[i]);

	free(arr);
	return 0;
}