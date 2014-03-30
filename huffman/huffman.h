#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "bitmap.h"
#include "types.h"
#include <stdio.h>

typedef struct Node_s {
	struct Node_s *l, *r;
	u8 c;
	int w; // weight
} Node;

typedef struct {
	u64 c;
	u64 len;
} Code;

#define MAX_INPUT_S_LEN (100000)
extern int huffmanEncode(FILE *in, FILE *out);
extern int huffmanDecode(FILE *in, FILE *out);

#endif
