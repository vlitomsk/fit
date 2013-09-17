#include <stdio.h>
#include <stdlib.h>

typedef enum {false, true} bool;
bool *is_hamming;
int *nth;

inline bool is_hamming_number(int n) {
	return is_hamming[n];
}

int nth_hamming_number(int n) {
	return nth[n];
}

void try_set(int index, int n, bool *cache) {
	if (index < n) 
		cache[index] = 1;
}

void fill_cache(int n, bool *cache) {
	int i, counter = 1;
	for (i = 1; i < n; ++i) {
		if (cache[i] == 1) {
			nth[counter++] = i;
			try_set(i * 2, n, cache);
			try_set(i * 3, n, cache);
			try_set(i * 5, n, cache);
		}
	}
}

void init(int cache_size) {
	is_hamming = (bool*)calloc(cache_size + 1, sizeof(bool));
	nth = (int*)malloc(sizeof(int) * cache_size);
	if (!is_hamming || !nth) {
		perror("Can't allocate memory\n");
		exit(1);
	}
	is_hamming[1] = 1;
	fill_cache(cache_size, is_hamming);
}

void finish() {
	free(is_hamming);
	free(nth);
}

int fastpow_(int base, int exp, int acc) {
	if (exp == 0)
		return acc;
	else if (exp & 1) 
		return fastpow_(base, exp - 1, acc * base);
	else
		return fastpow_(base * base, exp >> 1, acc);
}

int fastpow(int base, int exp) {
	return fastpow_(base, exp, 1);
}

main() {
	int max_n, i, k, l, m;

	init(max_n);
	printf("Enter k, l, m splitted by space: ");
	scanf("%d %d %d", &k, &l, &m);
	max_n = fastpow(2, k) * fastpow(3, l) * fastpow(5, m);

	printf("List of Hamming numbers under %d\n", max_n);
	for (i = 1; i < max_n; ++i) {
		if (is_hamming_number(i))
			printf("%d\n", i);
	}

	finish();
	return 0;
}
