#include "rkarp.h"
#include <string.h>

hash_t three_pows[] = {
	1,3,9,27,81,243,729,2187,6561,19683,59049,177147,531441,1594323,4782969,14348907,
	43046721,129140163
};

// в старшем разряде - последний символ
hash_t rkhash(const char *s, int len) {
	unsigned int hash = 0, i;
	for (i = 0; i < len; ++i) 
		hash += ((unsigned char)s[i] % 3) * three_pows[i];

	return hash;
}

/*
1) Сдвигаем все в сторону младшего разряда делением на 3
2) В старший разряд добавляем новый символ
*/
inline hash_t update_hash(hash_t *old_hash, char add_char, int hash_len) {
	*old_hash = (*old_hash / 3) + ((unsigned char)add_char % 3) * three_pows[hash_len - 1];
}

// +7 (383) 330 82 11
// Морской проспект, 25/1
int rksearch(const char *needle, const char *haystack, int pos) {
	int i, offset = 0, n_len = strlen(needle);
	int found_pos = -1;
	unsigned int n_hash = rkhash(needle, n_len),
				 h_hash = rkhash(haystack, n_len);

	while (offset + n_len < strlen(haystack) + 1) {
		if (n_hash == h_hash) {			
			for (i = 0; i < n_len; ++i) {
				printf("%d ", offset + i + pos);
				if (needle[i] != haystack[offset + i])
					break;
			}

			if (i == n_len - 1 && found_pos == -1)
				found_pos = offset;
		} 

		++offset;		
		update_hash(&h_hash, haystack[offset + n_len - 1], n_len);
	}
	
	return found_pos;
}
