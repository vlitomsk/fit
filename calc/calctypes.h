#ifndef CALC_TYPES_H
#define CALC_TYPES_H

#include <stdio.h>

typedef enum {false, true} bool;
typedef int Number;

extern inline Number parse_number(const char *s);
extern void number_to_str(Number n, char *s, int sz);
extern void p_number(Number n);

typedef enum {
	EvalOk = 0,
	SyntaxError = -1,
	DivByZero = -2,
	InvalidToken = -3,
	TokenOk = 1
} Status;

extern bool status_ok(Status s);

typedef struct {
	Number res;
	Status st;
} EvalRes;

extern void p_evalres(EvalRes r);
extern inline EvalRes evdone(Number res); // Evaluation OK
extern inline EvalRes everr(Status st); // Evaluation error

extern inline EvalRes number_add(Number a, Number b);
extern inline EvalRes number_sub(Number a, Number b);
extern inline EvalRes number_mul(Number a, Number b);
extern inline EvalRes number_div(Number a, Number b);
extern inline EvalRes number_neg(Number a, Number stub);

extern EvalRes binop_res(EvalRes a, EvalRes b, EvalRes (*binop)(Number, Number));
extern EvalRes add_res(EvalRes a, EvalRes b);
extern EvalRes sub_res(EvalRes a, EvalRes b);
extern EvalRes mul_res(EvalRes a, EvalRes b);
extern EvalRes div_res(EvalRes a, EvalRes b);
extern EvalRes neg_res(EvalRes a);

/* Token */

typedef enum {
	NUM = 1,
	END = 0,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '/',
	RP = ')',
	LP = '('
} TokenType;

typedef struct {
	TokenType t;
	union {
		Number val;
	};
	Status st;
} Token;

extern inline Token invtok(void);
extern inline Token numtok(Number n);
extern inline Token symtok(TokenType t);
extern void p_token(Token t);

#endif
