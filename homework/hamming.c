#include <stdio.h>
#include <stdlib.h>

unsigned cut_power(unsigned x, unsigned base) {
	while (!(x % base)) 
		x /= base;

	return x;
}

// O( log x )
int is_hamming_number(unsigned x) {
	x = cut_power(x, 2);
	x = cut_power(x, 3);
	x = cut_power(x, 5);
	return (x == 1);
}

unsigned next_hamming_number(unsigned from) {
	do {
		++from;
	} while (!is_hamming_number(from));
	return from;
}

// O( n * log n )
unsigned nth_hamming_number(int n) {
	unsigned from = 1, i;
	for (i = 1; i < n; ++i) 
		from = next_hamming_number(from);

	return from;
}

unsigned fastpow_(unsigned base, int exp, unsigned acc) {
	if (exp == 0)
		return acc;
	else if (exp & 1) 
		return fastpow_(base, exp - 1, acc * base);
	else
		return fastpow_(base * base, exp >> 1, acc);
}

unsigned fastpow(unsigned base, int exp) {
	return fastpow_(base, exp, 1);
}

main() {
	unsigned i, max_possible, k, l, m;
	printf("Enter k, l, m splitted by space: ");
	scanf("%d %d %d", &k, &l, &m);
	max_possible = fastpow(2, k) * fastpow(3, l) * fastpow(5, m);
	printf("List of Hamming numbers under %d: \n", max_possible);
	for (i = 1; i < max_possible; ++i) {
		if (is_hamming_number(i))
			printf("%d\n", i);
	}
	return 0;
}
