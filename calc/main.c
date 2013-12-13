#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include "calctypes.h"

/***********************/
/* Разбиение на токены */

#define MAX_TOKENS 1024

/* Обработка числового токена, используется в get_token */
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

void tokenize(const char *s, Token *tokens, int sz, int *count) {
	char *ptr = s;
	*count = 0;
	while (ptr && (*count) < sz) {
		tokens[(*count)++] = get_token(ptr, &ptr); // ДОБАВИТЬ ВЫКИДЫВАНИЕ НАВЕРХ INVALID TOKEN!!
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
	tokenize(s, tokens, MAX_TOKENS, &n);
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
	// printf("Expr :: ");p_token(*tok);
	int j;

	// пропускаем унарные минусы мимо
	for (j = i; j < n; ++j) {
		if (tok[j].t != MINUS)
			break;
	}

	int op_index = split(tok, j, n, expr_splitter);  // индекс токена оператора ("+"/"-")

	if (op_index == n) // оператора нет, тогда все выражение - терм
		return term(tok, i, n);

	EvalRes _term = term(tok, i, op_index), 
					_expr = expr(tok, op_index + 1, n);
	if (tok[op_index].t == PLUS) 
		return add_res(_term, _expr);
	else 
		return sub_res(_term, _expr);
}

EvalRes term(const Token *tok, int i, int n) {
	// printf("Term :: ");p_token(*tok);
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

#define EXPR_LEN (MAX_TOKENS)

int main() {
	char s_expr[EXPR_LEN];
	fgets(s_expr, EXPR_LEN, stdin);
	EvalRes res = eval(s_expr);
	p_evalres(res);	

	//Проверка разделения на токены
/*	char *tmp = "+";
	char *ptr = tmp;

	Token tok;
	while (ptr) {
		tok = get_token(ptr, &ptr);
		p_token(tok);
		if (tok.st == InvalidToken)
			break;
	}
*/	
	return 0;
}
