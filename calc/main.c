#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include "calc.h"

#define EXPR_LEN (MAX_TOKENS)

int main() {
	char s_expr[EXPR_LEN];
	fgets(s_expr, EXPR_LEN, stdin);
	EvalRes res = eval(s_expr);
	p_evalres(res);	

	return 0;
}
