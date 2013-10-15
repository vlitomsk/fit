#include "perm.h"
#include <stdio.h>
#include <string.h>

typedef enum {false, true} bool;
bool check_input(const char *perm) {
	int i, repeats[10];
	for (i = 0; i < 10; ++i)
		repeats[i] = 0;
	for (i = 0; i < strlen(perm); ++i) {
		if (perm[i] < '0' || perm[i] > '9')
			return false;
		if (repeats[perm[i] - '0'])
			return false;
		else
			repeats[perm[i] - '0'] = 1;
	}
	return true;
}

int main() {
	char perm[100];
	int n;
	scanf("%s\n%d", perm, &n);

	if (!check_input(perm)) {
		printf("bad input\n");
		return 0;
	}

	for (; n > 0; --n) {
		next_permutation(perm);
		printf("%s\n", perm);
	}
	
	return 0;
}
