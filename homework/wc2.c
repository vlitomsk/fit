#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* макс. длинаа слова, а также и бора */
#define MAX_WORD_LEN 255

typedef unsigned int u32;
typedef enum {false, true} bool;

u32 pows3[] = {
	1,3,9,27,81,243,729,2187,6561,19683,59049,177147,531441,
	1594323,4782969,14348907,43046721,129140163,387420489,
	1162261467,3486784401
};

/* Бор */
typedef struct {
	char c;
	Node_t *next = NULL;
	int count = 0; // количество слов, оканчивающихся на данном ребре
} Edge;

typedef struct Node_t {
	Edge *edges = NULL;
	int E = 0; // количество ребер 
} Node;

Node *add_edge(Node *node, char c) {
	Edge e = (Edge) {c, (Node) {NULL, 0}, 0};
	node->edges = (Edge*)realloc(node->edges, sizeof(Edge) * (node->E + 1));
	if (!node->edges) {
		perror("Can't allocate memory");
		return NULL;
	}
	
	node->edges[node->E++] = e;
	return node;
}

void add_to_trie(Node *t, const char *word) {
	int i;
	Edge *edges = t->edges;	
	for (i = 0; i < t->E; ++i) {
		if (edges[i].c == *word) 
			break;
	}

	if (i == t->E) // не нашлось ребро с *word
		add_edge(t, *word); 
	
	if (*(word + 1) == 0)
		edges[i].count++;
	else
		add_to_trie(edges[i].next, word + 1);
}

void count_traverse(const Node *trie, char *buf) {
	if (!trie)
		return;

	int i, L = strlen(buf);
	Edge *edges = trie->edges;
	for (i = 0; i < trie->E; ++i) {
		buf[L] = edges[i].c;
		if (edges[i].count != 0) 
			printf("Traverse: %s : %d times\n", buf, edges[i].count);
		count_traverse(edges[i].next, buf);
	}
	buf[L] = 0;
}

int max(int a, int b) {
	return (a > b ? a : b);
}

u32 hash(const char *s) {
	int i;
	int h = 0;
	for (i = 0; i < max(strlen(s), 32); ++i) {
		h += pows3[i] * (s[i] % 3);
	}
}

char trimmable_chars[] = ".,?!\":;'()";

bool trimmable(char ch) {
	return (strchr(trimmable_chars, ch) != NULL);
}

char* trim(char *w) {
	if (!trimmable(w[strlen(w) - 1]) && !trimmable(w[0]))
		return w;

	if (trimmable(w[strlen(w) - 1])) 
		w[strlen(w) - 1] = 0;

	return trim( trimmable(w[0]) ? w + 1 : w );
}

typedef struct {
	char *word;
	u32 hash;
	int count;
} WordInfo;

#define MAX_WORDS 100000

WordInfo new_info(const char *trimmed, u32 hash) {
	int bytes = sizeof(char) * strlen(trimmed);
	char *w = (char*)malloc(bytes);
	if (!w) {
		perror("Can't allocate bytes");
		return (WordInfo) {NULL, 0, 0};
	}
	memcpy(w, trimmed, bytes);

	return (WordInfo) {w, hash, 1};
}

void to_downcase(char *w) {
	while (*w) {
		*w = tolower(*w);
		w++;
	}
}

int main() {
	freopen("in.txt", "r", stdin);
	char word[MAX_WORD_LEN];

	WordInfo infos[MAX_WORDS];

	/*
	int i, wc = 0;
	while (scanf("%s", word) != EOF) {
		char *trimmed = trim(word);
		to_downcase(trimmed);
		u32 h = hash(trimmed);

		for (i = 0; i < wc; ++i) {
			if (infos[i].hash == h) 
				if (strcmp(trimmed, infos[i].word) == 0) {
					infos[i].count++;
					break;
				}
		}

		if (i == wc) 
			infos[wc++] = new_info(trimmed, h);
	}

	for (i = 0; i < wc; ++i) {
		printf("%s: %d\n", infos[i].word, infos[i].count);
		free(infos[i].word);
	}
	*/

	Node trie;

	/* Строим бор */
	while (scanf("%s", word) != EOF) {
		char *trimmed = trim(word);
		to_downcase(trimmed);
		add_to_trie(&trie, trimmed);
	}

	char buf[MAX_WORD_LEN]; // для обхода дерева
	memset(buf, 0, sizeof(char) * MAX_WORD_LEN);

	count_traverse(trie, buf);

	return 0;
}

