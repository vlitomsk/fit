#include "calc.h"
#include <ctype.h>
#include <stdlib.h>

/***********************/
/* Разбиение на токены */

/* Обработка числового токена, используется в get_token */
Token get_number_token(const char *s, char **ptr) {
	bool has_pt = false;
	int i;
	for (i = 0; s[i]; ++i) {
		if (!isdigit(s[i]))
			break;
	}
	*ptr = s + i ;

	return numtok(parse_number(s));
}

/* ptr - указывает на место, с которого можно получать следующий токен */
Token get_token(const char *s, char **ptr) {
	if (!s)
		return symtok(END);

	if (*s == 0) {
		*ptr = NULL;
		return symtok(END);
	}

	if (isspace(*s))
		return get_token(s + 1, ptr);

	if (isdigit(*s)) {
		return get_number_token(s, ptr);
	}

	if (*s == '+' || *s == '-' || *s == '*' ||  *s == '/' || *s == '(' || *s == ')') {
		*ptr = s + 1;
		return symtok(*s);
	}

	return invtok();
}

/*******************************/
/* Вычисление expr, term, prim */

/*** ГРАММАТИКА: ***/
/* Expr:           *
 *   Term + Expr   *
 *	 Term - Expr   *
 *	 Term          *
 *                 *
 * Term:           *
 *   Prim * Term   *
 *   Prim / Term   *
 *   Prim          *
 *                 *
 * Prim:           *
 *   (Expr)        *
 *   -Prim         *
 *   Number        *
 *******************/                

EvalRes expr(const char *s, char **s1, int d);
EvalRes term(const char *s, char **s1, int d);
EvalRes prim(const char *s, char **s1, int d);

EvalRes prim(const char *s, char **s1, int d) {
	// printf("Getting prim from `%s`\n", s);
	Token tok = get_token(s, s1);
	if (tok.st == InvalidToken)
		return everr(InvalidToken);

	if (tok.t == NUM) {
		return evdone(tok.val);
	}

	if (tok.t == MINUS) {
		char *s2 = *s1;
		Token tok2 = get_token(*s1, s1);
		if (tok2.t != NUM && tok2.t != LP) 
			return everr(SyntaxError);
		else
			*s1 = s2;
		return neg_res(prim(*s1, s1, d));
	}
	
	if (tok.t == LP) {
		EvalRes r = expr(*s1, s1, d + 1);
		tok = get_token(*s1, s1);
		if (tok.st == InvalidToken)
			return everr(InvalidToken);
		if (tok.t != RP) 
			return everr(SyntaxError);
		return r;
	} 

	 // puts("Returning syntax error from prim");
	return everr(SyntaxError);
}

EvalRes term(const char *s, char **s1, int d) {
  // printf("Getting term from `%s`\n", s);
	EvalRes p = prim(s, s1, d);
	char *s2 = *s1;
	Token sgn = get_token(*s1, s1);
	if (sgn.st == InvalidToken)
		return everr(InvalidToken);

	if (sgn.t == MUL)
		return mul_res(p, term(*s1, s1, d));
	else if (sgn.t == DIV)
		return div_res(p, term(*s1, s1, d));
	else if (sgn.t == END || sgn.t == RP || sgn.t == MINUS || sgn.t == PLUS) {
		if (sgn.t == RP && d <= 0)
			return everr(SyntaxError);
		*s1 = s2;
		return p;
	} else
		return everr(SyntaxError);
}

EvalRes expr(const char *s, char **s1, int d) {
	// printf("Getting expr from `%s`\n", s);
	EvalRes t = term(s, s1, d);
	if (t.st != EvalOk) 
		return t;
	char *s2 = *s1;
	Token sgn = get_token(*s1, s1);
	if (sgn.st == InvalidToken)
		return everr(InvalidToken);

	if (sgn.t == PLUS)
		return add_res(t, expr(*s1, s1, d));
	else if (sgn.t == MINUS) {
		*s1 = s2;
		return add_res(t, expr(*s1, s1, d));
	} else if (sgn.t == END || sgn.t == RP) {
		if (sgn.t == RP && d <= 0)
			return everr(SyntaxError);
		*s1 = s2;
		return t;
	} else
		return everr(SyntaxError);
}

/* самый абстрактный метод калькулятора */
inline EvalRes eval(const char *s) {
	return expr(s, &s, 0);
}
