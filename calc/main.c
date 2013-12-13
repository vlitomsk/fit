#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include "calctypes.h"

/*
	Expr:
		Term+Expr
		Term-Expr
		Term

	Term:
		Prim*Term
		Prim/Term
		Prim

	Prim:
		(Expr)
		Number
		-Prim
*/

Token get_number_token(const char *s, char **ptr) {
	bool has_pt = false;
	int i;
	for (i = 0; s[i]; ++i) {
		if (isdigit(s[i]))
			continue;

		if (s[i] == '.') {
			if (has_pt)
				return invtok();
			else
				has_pt = true;
		} else 
			break;
	}

	if (s[i] == 0) 
		*ptr = NULL;
	else
		*ptr = s + i;

	return numtok(parse_number(s));
}

/* ptr - указывает на место, с которого можно получать следующий токен */
Token get_token(const char *s, char **ptr) {
	//printf("ch = %d\n", *s);
	if (isspace(*s))
		return get_token(s + 1, ptr);

	if (*s == 0) {
		*ptr = NULL;
		return symtok(END);
	}

	if (isdigit(*s))
		return get_number_token(s, ptr);

	if (*s == '+' || *s == '-' || *s == '*' ||  *s == '/' || *s == '(' || *s == ')') {
		*ptr = s + 1;
		return symtok(*s);
	}

	return invtok();
}

Number eval_expr() {
	return 0; // stub
}

Number eval_term() {
	return 0; // stub
}

Number eval_prim() {
	return 0; // stub
}

#define EXPR_LEN 255
int main() {
	/*char s_expr[EXPR_LEN];
	fgets(s_expr, EXPR_LEN, stdin);
	Number res = eval_expr(s_expr);*/

	char *tmp = " \t-5 *   ((3.55  + 4 ) (7.55+2))(  ";
	char *ptr = tmp;

	Token tok;
	while (ptr) {
		tok = get_token(ptr, &ptr);
		print_token(tok);
		if (tok.st == InvalidToken)
			break;
	}

	return 0;
}
