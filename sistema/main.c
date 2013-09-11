#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"
#include "common.h"

int main() {
	int b1, b2;
	char *inp = (char*)calloc(16, sizeof(char));
	check_alloc(inp);

	scanf("%d %d\n", &b1, &b2);
	inp = fgets(inp, 15, stdin);
	inp[strlen(inp) - 1] = 0; // убирает ненужный пробел в конце строки
	char *res = translate(b1, b2, inp);
	printf("%s\n", res);

	free(res);
	free(inp);
	return 0;
}
