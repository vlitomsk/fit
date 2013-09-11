#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

typedef unsigned long long u64;
typedef enum {true, false} bool; 

#define ERROR -1
#define POINT -2

extern int digit_to_num_equiv(char);
extern int bounds(int x, int a, int b);
extern void xor_swap(char *a, char *b);
extern void reverse_string(char *s);
extern int digit_to_num_equiv(char d);
extern char num_to_digit_equiv(int n);
extern void check_alloc(void *ptr);
extern int min(int, int);

// не нашел универсального решения через printf
extern void print_ulong_long(u64 x); 

#endif