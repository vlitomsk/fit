#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"
#include "common.h"

int main() {
	int b1, b2;
	char inp[16];
	scanf("%d %d\n%s", &b1, &b2, inp);
	if (!validate_input(b1, b2, inp)) {
		printf("bad input\n");
		return 1;
	}

	char res[50];
	translate2(b1, b2, inp, res);
	printf("%s\n", res);

	return 0;
}
