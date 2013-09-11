#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "translator.h"
#include "common.h"

void inpbye() {
	printf("bad input\n");
	exit(1);
}

int* parse_number(char *inp, int from, int to) {
	int i, *parsed = (int*)malloc(sizeof(int) * (to - from));
	check_alloc(parsed);

	for (i = from; i < to; ++i) 
		parsed[i - from] = digit_to_num_equiv(inp[i]);

	return parsed;
}

#define NOTFOUND -1
// works
int** parse_input(int b1, int b2, char *inp, int *len1, int *len2) {
	if (!bounds(b1, 2, 16) || !bounds(b2, 2, 16))  
		inpbye();

	int i, pointpos = strlen(inp);
	char points = 0;
	for (i = 0; i < strlen(inp); ++i) {
		int equiv = digit_to_num_equiv(inp[i]);
		if (equiv == POINT) {
			++points;
			pointpos = i;
		} else if (equiv == ERROR || !bounds(equiv, 0, b1 - 1))
			inpbye();
	}

	if (points > 1)
		inpbye();

	int** parsed = (int**)malloc(sizeof(int*) * 2);
	check_alloc(parsed);
	parsed[0] = parse_number(inp, 0, pointpos);
	parsed[1] = (points ? parse_number(inp, pointpos + 1, strlen(inp)) : NULL);

	*len1 = pointpos;
	*len2 = (points ? strlen(inp) - pointpos - 1 : strlen(inp) - pointpos);

	return parsed;
}

/*
  основания b1 -> 10 -> b2 
  выделяет память под ответ
*/
char* int_translate(int b1, int b2, int *s, int slen) {
	u64 sum = 0;
	int i;
	for (i = slen - 1; i >= 0; --i)
		sum += (u64)s[i] * (u64)pow(b1, slen - 1 - i);
	char *output = (char*)calloc(12*4 + 1, sizeof(char));
	check_alloc(output);

	int ost;
	i = 0;
	while (sum) {
		ost = sum % (u64)b2;
		output[i++] = num_to_digit_equiv(ost);
		sum /= (u64)b2;
	}

	reverse_string(output);
	return output;
}

char* translate(int b1, int b2, char *s) {
	int len1, len2;
	int **parsed = parse_input(b1, b2, s, &len1, &len2);
	char *p1 = int_translate(b1, b2, parsed[0], len1);
	char *p2 = NULL;
	if (len2 != 0) 
		p2 = int_translate(b1, b2, parsed[1], len2);
	len1 = strlen(p1);
	len2 = (!len2 ? 0 : strlen(p2));

	char *concat = (char*)calloc((len1 + len2 + 1), sizeof(char));
	check_alloc(concat);
	concat = strcat(concat, p1);
	if (len2 != 0) {
		concat = strcat(concat, ".");
		concat = strncat(concat, p2, min(12, len2));
	}

	free(parsed[1]);
	free(parsed[0]);
	free(parsed);
	free(p1);
	free(p2);

	return concat;
}
