#include "bmoore.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define TABLE_LEN 256

void fill_stop_table(const char *needle, int *table) {
	int i;
	for (i = 0; i < TABLE_LEN; ++i)
		table[i] = -1;

	for (i = strlen(needle); i >= 0; --i) {
		char c = needle[i];
		if (table[c] < 0) {
			table[c] = i;
		}
	}
}

// offset - смещение строки needle в строке haystack
// возвращает длину совпавшего суффикса
int max_common_suffix(const char *haystack, const char *needle, int offset, char *stop_char) {
	if (offset + strlen(needle) > strlen(haystack))
		return -1;
	// printf("%d\n", offset);
	int i, len = 0;
	for (i = offset + strlen(needle) - 1; i >= offset; --i, ++len) {
		printf("%d ", i + 1);
		if (haystack[i] != needle[i - offset]) {
			printf("Not equal! Index = %d, char[haystack] = %c\n", i, haystack[i]);
			*stop_char = haystack[i];
			break;
		}
	}

	return len;
}

int bmoore_search(const char *haystack, const char *needle) {
	int nlen = strlen(needle);
	if (nlen > strlen(haystack))  {
		return -1;
	}
	// printf("Find %s in %s\n", needle, haystack);

	int stop_table[TABLE_LEN],
		*pi = (int*)malloc(sizeof(int) * strlen(needle)),
		offset = 0, stop_index, mcs, found_index = -1;
	char stop_char;
	pi_function(needle, pi);
	fill_stop_table(needle, stop_table);

	while (offset + nlen < strlen(haystack) + 1) {
		mcs = max_common_suffix(haystack, needle, offset, &stop_char);
		if (mcs == nlen) {
			printf("FOUND\n");
			found_index = (found_index < 0 ? offset : found_index);
			offset += nlen;
		} else if (mcs < 0) {
			offset = -1;
			break;
		} else {
			stop_index = stop_table[stop_char];
			if (stop_index < 0 && mcs == 0) {
				offset += nlen;
			} else if (stop_index > nlen - 1 - mcs || stop_index < 0) {
				offset += nlen - pi[nlen - 1];
			} else {
				offset += nlen - mcs - stop_index - 1;
			}
			if (offset + nlen > strlen(haystack)) {
				offset = -1;
				break;
			}
			printf("Offset: %d\n", offset);
		}
	} 
	putchar('\n');
	free(pi);
	return found_index;
}

void pi_function(const char *s, int *pi) {
	if (!strlen(s))
		return;

	int i;

	pi[0] = 0;
	for (i = 0; i < strlen(s) - 1; ++i) {
		if (s[ pi[i] ] == s[i + 1]) 
			pi[i + 1] = pi[i] + 1;
		else
			pi[i + 1] = 0;
	}
}
