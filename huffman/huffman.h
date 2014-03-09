#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "bitmap.h"
#include <stdio.h>

typedef struct Node_s {
	struct Node_s *l, *r;
	char c;
	int w; // weight
} Node;

typedef struct {
	u64 c;
	u32 len;
} Code;

#define MAX_INPUT_S_LEN (10000)
extern int huffmanEncode(FILE *in, FILE *out);

extern int huffmanDecode(FILE *in, FILE *out);

extern void fillTable(const char *s, Node *table, int table_size);
extern int findMinFrom(const Node *table, int table_size, int exclude_index);
extern Node* tableToTree(Node *table, int table_size);
extern void getCodes(const Node *t, Code *arr, u32 code, int code_len);
extern void encodeTree(const Node *t, Bitmap *out);

extern Node* buildTree(const char *s);
extern void freeTree(Node *t);
extern void printTree(const Node *t);

#endif
