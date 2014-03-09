#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "bitmap.h"

void passNewline() {
	while (getchar() != '\n')
		return;
}

int main(int argc, char **argv) {
	if (getchar() == 'c') { // Compress
		passNewline();
		huffmanEncode(stdin, stdout);
	} else { // Decompress
		passNewline();
		huffmanDecode(stdin, stdout);
	}

	return 0;
}
