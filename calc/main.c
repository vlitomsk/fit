#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include "calctypes.h"

/***********************/
/* Разбиение на токены */

#define MAX_TOKENS 1024

Token get_number_token(const char *s, char **ptr);
/* ptr - указывает на место, с которого можно получать следующий токен */
Token get_token(const char *s, char **ptr) {
	printf("ch = %d\n", *s);
	if (isspace(*s))
		return get_token(s + 1, ptr);

	if (*s == 0) {
		*ptr = NULL;
		return symtok(END);
	}

	if (isdigit(*s)) {
		return get_number_token(s, ptr);
	}

	if (*s == '+' || *s == '-' || *s == '*' ||  *s == '/' || *s == '(' || *s == ')') {
		*ptr = s + 1;
		return symtok(*s);
	}

	return invtok();
}

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
	// printf("str: %d\n", s[i - 1]);
	*ptr = s + i ;

	return numtok(parse_number(s));
}

void tokenize(const char *s, Token *tokens, int sz, int *count) {
	char *ptr = s;
	*count = 0;
	while (ptr && *count < sz)
		tokens[*count++] = get_token(ptr, &ptr);	
	// ДОАБВИТЬ ОБЯЗАТЕЛЬНО END!!
}

/*******************************/
/* Вычисление expr, term, prim */

/*** ГРАММАТИКА: ***/
/* Expr:
 *   Term + Expr
 *	 Term - Expr
 *	 Term
 * 
 * Term:
 *   Prim * Term
 *   Prim / Term
 *   Prim
 *
 * Prim:
 *   (Expr)
 *   -Prim
 *   Number
 */

EvalRes expr(const Token*, int, int);
EvalRes term(const Token*, int, int);
EvalRes prim(const Token*, int, int);

EvalRes eval(const char *s) {
	int n;
	Token tokens[MAX_TOKENS];
	tokenize(s, tokens, MAX_TOKENS, &n);
	return expr(tokens, 0, n);
}

EvalRes expr(const Token *tok, int i, int n) {
	p_token(*tok);
	int j;

	// разделяем по плюсу/минусу
	for (j = i; j < n; ++j) {
		if (tok[j].t == PLUS || tok[j].t == MINUS)
			break;
	}

	if (j == n) {
		puts("expr -> term");
		return term(tok, i, n);
	}

	EvalRes _term = term(tok, i, j), 
					_expr = expr(tok, j + 1, n);
	if (tok[j].t == PLUS) 
		return add_res(_term, _expr);
	else if (tok[j].t == MINUS)
		return sub_res(_term, _expr);
}

EvalRes term(const Token *tok, int i, int n) {
	p_token(*tok);
	int j;

	// разделяем по умножению/делению
	for (j = i; j < n; ++j) {
		if (tok[j].t == MUL || tok[j].t == DIV)
			break;
	}

	if (j == n) {
		puts("term -> prim");
		return prim(tok, i, n);
	}

	EvalRes _prim = prim(tok, i, j),
					_term = term(tok, j + 1, n);
	if (tok[j].t == MUL)
		return mul_res(_prim, _term);
	else if (tok[j].t == DIV)
		return mul_res(_prim, _term);
}

EvalRes prim(const Token *tok, int i, int n) {
	p_token(*tok);

	if (tok[i].t == MINUS) // unary minus
		return neg_res(prim(tok, i + 1, n));

	if (tok[i].t == LP) {
		// выделяем выражение для передачи в expr
		int j;
		int depth = 0;
		for (j = i; j < n; ++j) {
			if (tok[j].t == LP)
				++depth;
			else if (tok[j].t == RP)
				--depth;

			if (depth == 0)
				break;
		}

		if (depth != 0)
			return everr(SyntaxError); // Parentheses mismatch

		return expr(tok, i + 1, j);
	} 

	if (tok[i].t == NUM) 
		return evdone(tok[i].val);

	return everr(SyntaxError);
}

#define EXPR_LEN (MAX_TOKENS)

int main() {
	// char s_expr[EXPR_LEN];
	// fgets(s_expr, EXPR_LEN, stdin);
	// EvalRes res = eval(s_expr);
	// p_evalres(res);	

	 //Проверка разделения на токены
	char *tmp = "+";
	char *ptr = tmp;

	Token tok;
	while (ptr) {
		tok = get_token(ptr, &ptr);
		p_token(tok);
		if (tok.st == InvalidToken)
			break;
	}
	
	return 0;
}
