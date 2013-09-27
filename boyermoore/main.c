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

void test_pi_function() {
	char s[12] = "aaabaabaaab";
	int pi_ideal[11] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4},
	    pi_res[11], i;
	pi_function(s, pi_res);
	for (i = 0; i < 11; ++i) 
		assert(pi_ideal[i] == pi_res[i]);	
}

void test() {
	test_pi_function();
	test_bmoore();
}

main() {
	//test();

	char needle[17], haystack[1001];
	fgets(needle, 16, stdin);
	needle[strlen(needle) - 1] = 0;
	fread(haystack, sizeof(char), 1000, stdin);
	haystack[strlen(haystack) - 1] = 0;
	// printf("Needle: %s\nHaystack: %s\n", needle, haystack);
	bmoore_search(haystack, needle);
	//bmoore_search("this is simple example", "example");

	return 0;
}