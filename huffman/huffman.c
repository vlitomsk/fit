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
void fillTable(FILE *in, Node *table, int table_size) { // +сортирует таблицу
	u32 i;
	memset((void*)table, 0, sizeof(Node) * table_size);

	u8 ch;
	while (fread(&ch, sizeof(u8), 1, in)) {
		table[ch].c = ch;
		table[ch].w++;
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
	return (l_ind == table_size ? NULL : dupNode(table[l_ind]));
	// останется единственный узел с ненулевым весом
}

Node* buildTree(FILE *in) {
	Node table[ALPHABET_SZ];
	fillTable(in, table, ALPHABET_SZ);

/*	puts("Table: ");
	int i;
	for (i = 0; i < ALPHABET_SZ; ++i) {
		if (table[i].w)
			printf("0x%X -> %2d\n", table[i].c, table[i].w);
	}*/
	Node *tr = tableToTree(table, ALPHABET_SZ);

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

void printNode(const Node *l, int depth) {
	printf("DEPTH: %d\n", depth);
	if (isLeaf(l))
		printf("[%p] Leaf (c: `%x`, w: %2d)\n", l, l->c, l->w);
	else
		printf("[%p] Node (______, w: %2d, L: %p, R: %p)\n", l, l->w, l->l, l->r);
}

void printTree_(const Node *t, int depth) {
	if (!t)
		return;

	printNode(t, depth);

	printTree_(t->l, depth+1);
	printTree_(t->r, depth+1);
}

inline void printTree(const Node *t) {
	printTree_(t, 0);
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
void getCodes(const Node *t, Code *arr, u64 code, int code_len) {
	if (!t)
		return;
	if (isLeaf(t)) {
		arr[t->c] = (Code) {code, code_len};
		//printCode(t->c, arr[t->c]);
	}

	getCodes(t->l, arr, setBit(code, code_len, 0), code_len + 1);
	getCodes(t->r, arr, setBit(code, code_len, 1), code_len + 1);
}

void encodeTree(const Node *t, BRWState *bws, FILE *out) {
	if (!t)
		return;

	writeBitThr(isLeaf(t), bws, out);
	if (isLeaf(t))
		writeByteThr(t->c, bws, out);

	encodeTree(t->l, bws, out);
	encodeTree(t->r, bws, out);
}

void writeCode(Code c, BRWState *bws, FILE *out) {
	int i;
	for (i = 0; i < c.len; ++i) 
		writeBitThr((c.c >> i) & 1, bws, out);
}

inline u32 bytes_round(u64 bits) {
	if (bits % 8 == 0)
		return bits >> 3;
	return 1 + (bits >> 3);
}

int encodeMsg(FILE *in, Code *codes, BRWState *bws, FILE *out) {
	u8 ch;
	while (fread(&ch, sizeof(u8), 1, in)) {
		writeCode(codes[ch], bws, out);
	}
}

u64 calcTreeTextSize(const Node *tr, u64 acc, int path_len) {
	if (!tr)
		return 0;
	
	if (isLeaf(tr)) {
		return acc + 8 + tr->w*path_len;
		// 8 (simvol v liste) + ves*path_len
	}
	
	return calcTreeTextSize(tr->l, acc + 1, path_len + 1) +
	       calcTreeTextSize(tr->r, acc + 1, path_len + 1);
}

// in bits
inline u64 calcCompressSize(const Node *tr) {
	return 32 + calcTreeTextSize(tr, 0, 0); // 32 na razmer faila
}

int huffmanEncode(FILE *in, FILE *out) {
	u8 ch;
	int pos = ftell(in);
	
	Node *tr = buildTree(in); // buildTree peremativaet in v konec
	u32 len = ftell(in) - pos;
	u64 csize = calcCompressSize(tr);
	if (csize >> 3 >= len + 4) { // text uvelichitsa posle haffmana
		fputs("\xff\xff\xff\xff", out);
		fseek(in, pos, SEEK_SET);
		while (fread(&ch, sizeof(u8), 1, in)) {
			fputc(ch, out);
		}
	} else { // text sozhmetsa posle haffmana
		Code codes[ALPHABET_SZ];
		memset(codes, 0, sizeof(Code) * ALPHABET_SZ);
		getCodes(tr, codes, 0, 0);
		BRWState *bws = initBRWS();
		writeDWordThr(len, bws, out);	
		encodeTree(tr, bws, out);
		fseek(in, pos, SEEK_SET);
		encodeMsg(in, codes, bws, out);
		
		flushBitWriter(bws, out);
		free(bws);
	}
	
	freeTree(tr);
	
	return 0;
}

Node *decodeTree(BRWState *brs, FILE *in) {
	Node *node = (Node*)malloc(sizeof(Node));
	assert(node);
	int ok;
	u8 bit = readBitThr(brs, in, &ok);
	
	if (bit) { // leaf		
		node->c = readByteThr(brs, in, &ok);
		node->w = 1; // теперь не имеет значения
		node->l = node->r = NULL;
	} else {
		node->c = node->w = 0;
		node->l = decodeTree(brs, in);
		node->r = decodeTree(brs, in);
	}

	return node;
}

u8 extractNextChar(BRWState *brs, FILE *in, const Node *tr) {
	if (!tr)
		return 0; // какая-то загадочная ошибка

	int ok;
	if (isLeaf(tr))
		return tr->c;
	
	if (!readBitThr(brs, in, &ok)) // 0 -- idti nalevo 
		return extractNextChar(brs, in, tr->l);

	return extractNextChar(brs, in, tr->r); 
	// posle togo, kak shodili nalevo, idem napravo
}

int huffmanDecode(FILE *in, FILE *out) {
	BRWState *brs = initBRWS();
	u8 byte;
	int ok = 1;
	u32 len, i;
	
	len = readDWordThr(brs, in, &ok);
	if (len != 0xffffffff) { // compression was used
		Node *tr = decodeTree(brs, in);
		
		for (i = 0; i < len; ++i) {
			byte = extractNextChar(brs, in, tr);
			fputc(byte, out);
		}
		freeTree(tr);
	} else { // => compression wasn't used
		while (1) {
			byte = readByteThr(brs, in, &ok);
			if (!ok)
				break;
			fputc(byte, out);
		}
	}

	free(brs);
	fflush(out);
	return 0;
}
