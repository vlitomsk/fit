#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "bmoore.h"

#define T(haystack, needle, retval) (assert(bmoore_search((haystack), (needle)) == (retval)))
void test_bmoore() {
	T("a", "aa", -1);
	T("ba", "aa", -1);
	T("ba", "aa", -1);
	T("vitokolokol", "kolokol", 4);
	T("tolotolokol", "tolokol", 4);
	T("tolotolotolokol", "tolokol", 8);
	T("xxxkkolokol", "kolokol", 4);
	T("kolkkolokol", "kolokol", 4);
	T("abcdefgh", "abcd", 0);
	T("abcdeabcdfmnk", "abcdq", -1);
	T("labcdelabcdfmnk", "abcdq", -1);
	T("lababqlababcd", "ababc", 7);
	T("lmnkqrabcd", "abcd", 6);
	T("xxxkokolbbbbolookol", "olookol", 12);
	T("this is simple example", "example", 15);
}

void test() {
	test_bmoore();
}

void newline_trim(char *s) {
	int l = strlen(s);
	if (s[l - 1] == '\n')
		s[l - 1] = 0;
	l = strlen(s);
	if (s[l - 1] == '\r')
		s[l - 1] = 0;
}

#define HAYSTACK_SZ 100
#define NEEDLE_SZ 20

main() {
	//test();

	char needle[NEEDLE_SZ], haystack[HAYSTACK_SZ];
	fgets(needle, NEEDLE_SZ - 1, stdin);
	newline_trim(needle);

	while (fread(haystack, sizeof(char), HAYSTACK_SZ - 1, stdin)) {
		bmoore_search(haystack, needle);
	}

	return 0;
}