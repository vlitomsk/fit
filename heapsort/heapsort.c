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

/* * * * * * * * * * * * * * * * * * * */

void sift(int *h, int n, int i) {
	int max_val, lnode, rnode, node;
	while (1) {
		node = get(h, n, i);
		lnode = get(h, n, l_index(i));
		rnode = get(h, n, r_index(i));

		max_val = max3(node, lnode, rnode);

		if (max_val == node)
			break;
		if (max_val == lnode) {
			swap(&h[i], &h[l_index(i)]);
			i = l_index(i);
		} else if (max_val == rnode) {
			swap(&h[i], &h[r_index(i)]);
			i = r_index(i);
		}
	}
}

void array_to_heap(int *arr, int n) {
	int i;
	for (i = n / 2; i >= 0; --i) {
		sift(arr, n, i);
	}
}

void heapsort(int *arr, int n) {
	int i;
	array_to_heap(arr, n);
	for (i = n - 1; i >= 1; --i) {
		swap(&arr[i], &arr[0]);
		sift(arr, i, 0);
	}	
}

