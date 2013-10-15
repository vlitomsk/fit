#include "heapsort.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void swap(int *a, int *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

inline int l_index(int node) {
	return node * 2 + 1;
}

inline int r_index(int node) {
	return node * 2 + 2;
}

int max2(int a, int b) {
	return (a > b ? a : b); 
}

int max3(int a, int b, int c) {
	return max2(a, max2(b, c));
}

int get(const int *a, int len, int i) {
	if (i >= len) 
		return INT_MIN;
	return a[i];
}

void printarr(const char *name, int *arr, int n) {
	int i;
	printf("%s:\n", name);
	for (i = 0; i < n; ++i)
		printf("  %d\n", arr[i]);
	putchar('\n');
}

/* * * * * * * * * * * * * */
/*     LOGIC HERE          */
/* * * * * * * * * * * * * */

void sift(int *h, int n, int i) {
	int max_val, node, lnode, rnode;
	do {
		node = get(h, n, i);
		lnode = get(h, n, l_index(i));
		rnode = get(h, n, r_index(i));

		max_val = max3(node, lnode, rnode);
		
		//printf("N: %d, L: %d, R: %d\n", node, lnode, rnode);
		if (max_val == lnode) {
			//puts("Swapping with left");
			swap(&h[i], &h[l_index(i)]);
			i = l_index(i);
		} else if (max_val == rnode) {
			//puts("Swapping with right");
			swap(&h[i], &h[r_index(i)]);
			i = r_index(i);
		}
	} while (max_val != node);
}

void array_to_heap(const int *arr, int n, int *heap) {
	int i;
	memcpy(heap, arr, sizeof(int) * n);
	for (i = n / 2; i >= 0; --i) {
		//printarr("Heap", heap, n);
		sift(heap, n, i);
	}
	//printarr("Heap", heap, n);
}

int heap_extract_max(int *heap, int n) {
	int max = heap[0];
	heap[0] = heap[n - 1];
	sift(heap, n - 1, 0);

	return max;
}

void heapsort(const int *arr, int n, int *out) {
	int i;
	int *heap = (int*)malloc(sizeof(int) * n);
	if (!heap) {
		perror("Can't allocate bytes!\n");
		return;
	}
	array_to_heap(arr, n, heap);
	for (i = 0; i < n; ++i) {
		out[n - i - 1] = heap_extract_max(heap, n - i);
	}	

	free(heap);
}

