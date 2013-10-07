#ifndef RABINKARP_H_INCLUDED
#define RABINKARP_H_INCLUDED

typedef unsigned int hash_t;
extern int rksearch(const char *needle, const char *haystack);
extern hash_t rkhash(const char *s, int len);

#endif