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

main() {
	//test();

	char needle[18], haystack[1001];
	fgets(needle, 18, stdin);

	needle[strlen(needle) - 1] = 0;
	if (needle[strlen(needle) - 1] == '\r')
		needle[strlen(needle) - 1] = 0;
	
/*	fread(haystack, sizeof(char), 1000, stdin);
	if (haystack[strlen(haystack) - 1] == '\n')
		haystack[strlen(haystack) - 1] = 0;
	bmoore_search(haystack, needle);
*/

	while (fgets(haystack, 1000, stdin)) {
		fgets(haystack, 1000, stdin);
		// printf("Needle: %s\nHaystack: %s\n", needle, haystack);
		if (haystack[strlen(haystack) - 1] == '\n')
			haystack[strlen(haystack) - 1] = 0;
		if (haystack[strlen(haystack) - 1] == '\r')
			haystack[strlen(haystack) - 1] = 0;
		bmoore_search(haystack, needle);
	}

	return 0;
}