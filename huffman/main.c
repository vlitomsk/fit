#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "bitmap.h"

void passNewline(FILE *f) {
	int ch = fgetc(f);
	if (ch == '\r')
		fgetc(f);
}

int main(int argc, char **argv) {	
	freopen("in.txt", "rb", stdin);
	freopen("out.txt", "wb", stdout);
	
	if (getchar() == 'c') { // Compress
		passNewline(stdin);
		huffmanEncode(stdin, stdout);
	} else { // Decompress
		passNewline(stdin);
		huffmanDecode(stdin, stdout);
	}

	return 0;
}
