#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

inline int bounds(int x, int a, int b) {
	return (x >= a && x <= b);
}

// Переводит остаток от деления (%2..%16) в письменный вид
char num_to_digit_equiv(int ost) {
	if (bounds(ost, 0, 9))
		return ost + '0';
	else if (bounds(ost, 10, 15))
		return ost - 10 + 'a';
}

void xor_swap(char *a, char *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

// действует по месту
void reverse_string(char *s) {
	int i, len = strlen(s);
	for (i = 0; i < len / 2; ++i) 
		xor_swap(&s[i], &s[len - i - 1]);
}

int digit_to_num_equiv(char x) {
	if (x == '.')
		return POINT;
	else if (bounds(x, '0', '9'))
		return x - '0';
	else if (bounds(x, 'a', 'f'))	
		return x - 'a' + 10;
	else
		return ERROR;
}

void print_ulong_long(u64 x) {
	int ost, i = 0,
		*arr = (int*)malloc(sizeof(int) * 10); // с запасом
	check_alloc(arr);

	while (x) {
		ost = (int)(x % 10000000);
		x /= 10000000;
		arr[i++] = ost;
	}

	for (--i; i >= 0; --i) 
		printf("%d", arr[i]);

	free(arr);
}

void check_alloc(void *ptr) {
	if (!ptr) {
		perror("Can't allocate memory");
		exit(1);
	}
}

int min(int a, int b) {
	return (a < b ? a : b);
}
