#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "types.h"
#include "huffman.h"
#include "bitmap.h"

#define ALPHABET_SZ 256

/*
typedef struct Node_s {
	struct Node_s *l, *r;
	char c;
	int w; // weight
} Node;
*/

int cmpNode(const void *n1, const void *n2) {
	return ((const Node*)n1)->w - ((const Node*)n2)->w;
}

/* table: индекс соответствует символу, в структуре по этому индексу - частота встречаемости */
void fillTable(const char *s, Node *table, int table_size) { // +сортирует таблицу
	int i;
	memset((void*)table, 0, sizeof(Node) * table_size);
	for (i = 0; i <= strlen(s); ++i) {
		table[s[i]].c = s[i];
		table[s[i]].w++;
	}

	qsort((void*)table, table_size, sizeof(Node), cmpNode);
}

/* Ищет узел с минимальным ненулевым весом 
	 Возвращает индекс, если найдено, иначе - table_size */
int findMinFrom(const Node *table, int table_size, int exclude_index) {
	int i, min = INT_MAX, min_index = table_size;
	for (i = 0; i < table_size; ++i) {
		if (table[i].w && table[i].w < min && i != exclude_index) {
			min = table[i].w;
			min_index = i;
		}
	}

	return min_index;
}

Node* dupNode(Node n) {
	Node *res = (Node*)malloc(sizeof(Node));
	assert(res);
	*res = n;
	return res;
}

/* На вход поступает отсортированная таблица частотности (возможно, с нулевыми частотами) 
	 из fillFreqTable
	 Выделяет доп. память */
Node* tableToTree(Node *table, int table_size) {
	int l_ind, r_ind;
	while (1) {
		l_ind = findMinFrom(table, table_size, -1);
		r_ind = findMinFrom(table, table_size, l_ind);
		if (r_ind == table_size) 
			break;
		
		Node *l = dupNode(table[l_ind]);
		Node *r = dupNode(table[r_ind]);
		Node parent = (Node) { l, r, 0, l->w + r->w };
		table[l_ind].w = 0;
		table[r_ind] = parent;
	}

	return dupNode(table[l_ind]); // останется единственный узел с ненулевым весом
}

Node* buildTree(const char *s) {
	Node table[ALPHABET_SZ];
	//Node *table = (Node*)malloc(sizeof(Node) * ALPHABET_SZ);
	//memset(table, 0, sizeof(Node) * ALPHABET_SZ);
	fillTable(s, table, ALPHABET_SZ);

	/*puts("Table: ");
	int i;
	for (i = 0; i < ALPHABET_SZ; ++i) {
		if (table[i].w)
			printf("%c -> %2d\n", table[i].c, table[i].w);
	}*/
	Node *tr = tableToTree(table, ALPHABET_SZ);
	//free(table);
	return tr;
}

void freeTree(Node *t) {
	if (!t)
		return;

	freeTree(t->l);
	freeTree(t->r);
	free(t);
}

/*
typedef struct {
	u32 c;
	u32 len;
} Code;
*/

inline bool isLeaf(const Node *t) {
	return (!t->l && !t->r);
}

void printNode(const Node *l) {
	if (isLeaf(l))
		printf("[%p] Leaf (c: `%c`, w: %2d)\n", l, l->c, l->w);
	else
		printf("[%p] Node (______, w: %2d, L: %p, R: %p)\n", l, l->w, l->l, l->r);
}

void printTree(const Node *t) {
	if (!t)
		return;

	printNode(t);

	printTree(t->l);
	printTree(t->r);
}

void printCode(int ch, Code c) {
	printf("%c -> ", ch);
	int i;
	for (i = 0; i < c.len; ++i) 
		printf("%d", (c.c >> i) & 1);
	puts("");
}

// Обход дерева в глубину и построение таблицы кодов
// arr_len - выходной параметр
// code - размером не меньше высоты дерева, code_len изначально 0
// arr должен подгоняться чистый (одни нули)
void getCodes(const Node *t, Code *arr, u32 code, int code_len) {
	if (!t)
		return;
	if (isLeaf(t)) {
		arr[t->c] = (Code) {code, code_len};
		//printCode(t->c, arr[t->c]);
	}

	getCodes(t->l, arr, setBit(code, code_len, 0), code_len + 1);
	getCodes(t->r, arr, setBit(code, code_len, 1), code_len + 1);
}

void encodeTree(const Node *t, Bitmap *out) {
	if (!t)
		return;

	writeBit(isLeaf(t), out);
	if (isLeaf(t))
		writeByte(t->c, out);

	encodeTree(t->l, out);
	encodeTree(t->r, out);
}

void writeCode(Code c, Bitmap *out) {
	int i;
	for (i = 0; i < c.len; ++i) 
		writeBit((c.c >> i) & 1, out);
}

void encodeMsg(const char *s, Code *codes, Bitmap *out) {
	int i;
	for (i = 0; i <= strlen(s); ++i)
		writeCode(codes[s[i]], out);
	writeBit(1, out);
}

int huffmanEncode(FILE *in, FILE *out) {
	Code codes[ALPHABET_SZ];
	char s[MAX_INPUT_S_LEN];
	memset(codes, 0, sizeof(Code) * ALPHABET_SZ);	
	memset(s, 0, sizeof(char) * MAX_INPUT_S_LEN);
	char ch; 
	int len = 0;
	while ((ch = getc(in)) != EOF) {
		s[len++] = ch;
	}
	//char s[] = "clowns has glowing crowns";
	Node *tr = buildTree(s);
	getCodes(tr, codes, 0, 0);

	Bitmap *out_bitmap = newBitmap();
	encodeTree(tr, out_bitmap);
	//sprintBitmap(out_bitmap);
	encodeMsg(s, codes, out_bitmap);
	//sprintBitmap(out_bitmap);

	printBitmap(out_bitmap, out);

	freeTree(tr);
	freeBitmap(out_bitmap);
	return 0;
}

Node *decodeTree(Bitmap *bmap) {
	Node *node = (Node*)malloc(sizeof(Node));
	assert(node);
	int err;
	u8 bit = readBit(bmap, &err);
	if (bit) { // leaf		
		node->c = readByte(bmap, &err);
		node->w = 1; // теперь не имеет значения
		node->l = node->r = NULL;
	} else {
		node->c = node->w = 0;
		node->l = decodeTree(bmap);
		node->r = decodeTree(bmap);
	}

	return node;
}

char extractNextChar(Bitmap *bmap, const Node *tr) {
	if (!tr)
		return 0; // какая-то загадочная ошибка

	int err;
	if (isLeaf(tr))
		return tr->c;
	
	if (!readBit(bmap, &err)) 
		return extractNextChar(bmap, tr->l);

	return extractNextChar(bmap, tr->r);
}

int huffmanDecode(FILE *in, FILE *out) {
	Bitmap *bmap = newBitmap();
	u8 byte;
	while (fread(&byte, sizeof(u8), 1, in)) 
		writeByte(byte, bmap);

	Node *tr = decodeTree(bmap);
	char ch;
	while (ch = extractNextChar(bmap, tr))
		fputc(ch, out);
	fflush(out);

	freeBitmap(bmap);
	freeTree(tr);
	return 0;
}
