#ifndef CALC_TYPES_H
#define CALC_TYPES_H

typedef enum {false, true} bool;
typedef double Number;

void number_to_str(Number n, char *s, int sz) {
	snprintf(s, sz, "%f", n);
}

void p_number(Number n) {
	printf("%f", n);
}

typedef enum {
	EvalOk = 0,
	SyntaxError = -1,
	DivByZero = -2,
	InvalidToken = -3,
	TokenOk = 1
} Status;

bool statusOk(Status s) {
	return (s >= 0);
}

typedef struct {
	Number res;
	Status st;
} EvalRes;

void p_evalres(EvalRes r) {
	if (r.st == SyntaxError)
		printf("syntax error\n");
	else if (r.st == DivByZero)
		printf("division by zero\n");
	else
		p_number(r.res); 
}

inline EvalRes evdone(Number res) { // Evaluation OK
	return (EvalRes) { res, EvalOk }; 
}

inline EvalRes everr(Status st) { // Evaluation error
	return (EvalRes) { (Number)0, st };
}

inline Number parse_number(const char *s) {
	return atof(s);
}

inline EvalRes number_add(Number a, Number b) {
	return evdone(a + b);
}

inline EvalRes number_sub(Number a, Number b) {
	return evdone(a - b);
}

inline EvalRes number_mul(Number a, Number b) {
	return evdone(a * b);
}

inline EvalRes number_div(Number a, Number b) {
	if (b == 0)
		return everr(DivByZero);
	return evdone(a / b);
}

inline EvalRes number_neg(Number a, Number stub) {
	return evdone(-a);
}

EvalRes binop_res(EvalRes a, EvalRes b, EvalRes (*binop)(Number, Number)) {
	if (a.st == SyntaxError || b.st == SyntaxError)
		return everr(SyntaxError);
	if (a.st == DivByZero || b.st == DivByZero)
		return everr(DivByZero);
	return binop(a.res, b.res);
}

EvalRes add_res(EvalRes a, EvalRes b) {
	return binop_res(a, b, number_add);
}

EvalRes sub_res(EvalRes a, EvalRes b) {
	return binop_res(a, b, number_sub);
}

EvalRes mul_res(EvalRes a, EvalRes b) {
	return binop_res(a, b, number_mul);
}

EvalRes div_res(EvalRes a, EvalRes b) {
	return binop_res(a, b, number_div);
}

EvalRes neg_res(EvalRes a) {
	return binop_res(a, a, number_neg);
}

typedef enum {
	NUM = 1,
	END = 0,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '/',
	RP = '(',
	LP = ')'
} TokenType;

typedef struct {
	TokenType t;
	union {
		Number val;
	};
	Status st;
} Token;

inline Token invtok(void) {
	return (Token) { END, 0, InvalidToken };
} 

inline Token numtok(Number n) {
	return (Token) { NUM, n, TokenOk };
}

inline Token symtok(TokenType t) {
	return (Token) { t, 0, TokenOk };
}

void p_token(Token t) {
	if (t.st == TokenOk) {
		if (t.t == END)
			printf("Token: END\n");
		else if (t.t == NUM) 
			printf("Token: %f\n", t.val);
		else 
			printf("Token: %c\n", (char)t.t);
	} else
		printf("Token: INVALID TOKEN\n");
}

#endif
