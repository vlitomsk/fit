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

// возвращает 1 в случае InvalidToken, 0 в случае успеха
int tokenize(const char *s, Token *tokens, int sz, int *count) {
	char *ptr = s;
	*count = 0;
	while (ptr && (*count) < sz) {
		Token tok =	get_token(ptr, &ptr);
		if (tok.st == InvalidToken)
			return 1;
		tokens[(*count)++] = tok;
	}
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

EvalRes expr(const Token*, int, int);
EvalRes term(const Token*, int, int);
EvalRes prim(const Token*, int, int);

/* самый абстрактный метод калькулятора */
EvalRes eval(const char *s) {
	int n;
	Token tokens[MAX_TOKENS];
	if (tokenize(s, tokens, MAX_TOKENS, &n))
		return everr(InvalidToken);
	return expr(tokens, 0, n - 1); // n-1 <=> без END
}

inline bool expr_splitter(Token t) {
	return (t.t == PLUS || t.t == MINUS);
}

inline bool term_splitter(Token t) {
	return (t.t == MUL || t.t == DIV);
}

/* Отщепляет Term от Expr, Prim от Term 
	Возвращает индекс токена, заданного через splitter */
int split(const Token *tok, int i, int n, bool (*splitter)(Token)) {
	int j, depth = 0;
	for (j = i; j < n; ++j) {
		if (tok[j].t == LP)
			++depth;
		else if (tok[j].t == RP)
			--depth;

		// отщепляем на самом верхнем уровне вложенности
		if (depth == 0 && splitter(tok[j])) 
			break;
	}
	return j;
}

EvalRes expr(const Token *tok, int i, int n) {
	int j;
	//printf("Expr ");p_token(*tok);
	// пропускаем унарные минусы мимо
	int minus_cnt = 0;
	for (j = i; j < n; ++j) {
		if (tok[j].t != MINUS)
			break;
		++minus_cnt;
	}
	if (minus_cnt > 1) 
		return everr(SyntaxError);

	int op_index = split(tok, j, n, expr_splitter);  // индекс токена оператора ("+"/"-")

	if (op_index == n) // оператора нет, тогда все выражение - терм
		return term(tok, i, n);

	EvalRes _term = term(tok, i, op_index), 
					_expr = expr(tok, op_index + 1, n);
	if (tok[op_index].t == PLUS) 
		return add_res(_term, expr(tok, op_index + 1, n));
	else {
		return add_res(_term, expr(tok, op_index, n));
	}
}

int pow(int a, int x) {
	if (x == 0)
		return 1;
	else
		return a * pow(a, x - 1);
}

int pow_(int a, int x, int acc) {
	if (x == 0)
		return acc;
	else
		return pow_(a, x - 1, acc * a);
}

EvalRes term(const Token *tok, int i, int n) {
	//printf("Term ");p_token(*tok);
	int op_index = split(tok, i, n, term_splitter);

	if (op_index == n) 
		return prim(tok, i, n);

	EvalRes _prim = prim(tok, i, op_index),
					_term = term(tok, op_index + 1, n);
	if (tok[op_index].t == MUL)
		return mul_res(_prim, _term);
	else
		return div_res(_prim, _term);
}

EvalRes prim(const Token *tok, int i, int n) {
	//printf("Prim ");p_token(*tok);
	if (tok[i].t == MINUS) 
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

		// j != n - 1, то в таком случае внутри Prim есть что-то правее (Expr)
		if (depth != 0 || j != n - 1) 
			return everr(SyntaxError); 

		return expr(tok, i + 1, j);
	} 

	// i != n - 1, то в таком случае внутри Prim есть что-то правее Number
	if (tok[i].t == NUM && i == n - 1) 
		return evdone(tok[i].val);

	return everr(SyntaxError);
}
