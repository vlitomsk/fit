#ifndef CALC_TYPES_H
#define CALC_TYPES_H

typedef enum {false, true} bool;
typedef double Number;

void number_to_str(Number n, char *s, int sz) {
	snprintf(s, sz, "%f", n);
}

inline Number parse_number(const char *s) {
	return (Number)atof(s);
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

inline EvalRes evdone(Number res) { // Evaluation OK
	return (EvalRes) { res, EvalOk }; 
}

inline EvalRes everr(Status st) { // Evaluation error
	return (EvalRes) { (Number)0, st };
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

void print_token(Token t) {
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
