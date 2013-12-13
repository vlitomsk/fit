#include "calctypes.h"

inline Number parse_number(const char *s) {
	return atoi(s);
}

void number_to_str(Number n, char *s, int sz) {
	snprintf(s, sz, "%d", n);
}

void p_number(Number n) {
	printf("%d", n);
}

bool status_ok(Status s) {
	return (s >= 0);
}

void p_evalres(EvalRes r) {
	if (r.st == SyntaxError || r.st == InvalidToken)
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
	puts("division");
	if (b == 0)
		return everr(DivByZero);
	return evdone(a / b);
}

inline EvalRes number_neg(Number a, Number stub) {
	return evdone(-a);
}

EvalRes binop_res(EvalRes a, EvalRes b, EvalRes (*binop)(Number, Number)) {
	/* Приоритет синтаксической ошибки выше */
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
