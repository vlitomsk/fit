#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "translator.h"
#include "common.h"

int validate_input(int b1, int b2, char *inp) {
	if (!bounds(b1, 2, 16) || !bounds(b2, 2, 16)) 
		return 0;

	if (strchr(inp, '.') != strrchr(inp, '.')) 
		return 0;

	int i;
	for (i = 0; i < strlen(inp); ++i) {
		int equiv = digit_to_num_equiv(inp[i]);
		if (equiv == POINT)
			continue;
		else if (equiv == ERROR || !bounds(equiv, 0, b1 - 1)) 
			return 0;
	}

	return 1;
}

void translate_int_part(int base, u64 n, char *output) {
	int ost, i = 0;
	while (n) {
		ost = n % (u64)base;
		output[i++] = num_to_digit_equiv(ost);
		n /= (u64)base;
	}
	output[i] = 0;

	reverse_string(output);	
}

#define EPS 1e-30
void translate_frac_part(int base, double frac, char *output) {
	int i;
	for (i = 0; i < 12; ++i) {
		frac *= (double)base;
		output[i] = num_to_digit_equiv((int)floor(frac));
		frac -= floor(frac);
	}

	for (i = 11; i >= 0; --i) {
		if (output[i] != '0')
			break;
	}
	output[i + 1] = 0;
}

void translate(int b1, int b2, char *s, char *output) {
	int i, pt_index = (int)strchrnul(s, '.') - (int)s;
	double sum = 0;
	for (i = 0; i < strlen(s); ++i) {
		if (s[i] == '.') 
			continue;

		double eq = (double)digit_to_num_equiv(s[i]);
		double p = pow(b1, pt_index - i - 1);
		if (i >= pt_index)
			p = pow(b1, pt_index - i);
		sum += eq * p;
	}

	char int_part[49], frac_part[13];
	translate_int_part(b2, (u64)floor(sum), int_part);
	output[0] = 0;
	strcat(output, int_part);
	if (pt_index != strlen(s)) {
		translate_frac_part(b2, sum - floor(sum), frac_part);
		strcat(output, ".");
		strcat(output, frac_part);
	}
}
