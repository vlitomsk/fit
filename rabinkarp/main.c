#include "rkarp.h"
#include <stdio.h>
#include <string.h>

#define HAYSTACK_SZ 100
#define NEEDLE_SZ 20

void newline_trim(char *s) {
	int l = strlen(s);
	if (s[l - 1] == '\n')
		s[l - 1] = 0;
	l = strlen(s);
	if (s[l - 1] == '\r')
		s[l - 1] = 0;
}

int main() {
	char needle[NEEDLE_SZ], haystack[HAYSTACK_SZ];
	fgets(needle, NEEDLE_SZ - 1, stdin);
	newline_trim(needle);
	int pos = 1;

	printf("%d ", rkhash(needle, strlen(needle)));

	// while (fread(haystack, sizeof(char), HAYSTACK_SZ - 1, stdin)) {
	while (fgets(haystack, HAYSTACK_SZ - 1, stdin)) {
		rksearch(needle, haystack, pos);
		pos += strlen(haystack);
	}

	return 0;
}

