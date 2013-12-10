#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum {false, true} bool;

/* макс. длинаа слова, а также и бора */
#define MAX_WORD_LEN 255

/* Бор */
typedef struct {
	char c;
	struct Node_t *next;
	int count; // количество слов, оканчивающихся на данном ребре
} Edge;

typedef struct Node_t {
	Edge *edges;
	int E; // количество ребер 
} Node;

inline Node init_node(void) {
	return (Node) {NULL, 0};
}

Node trie;

Node *add_edge(Node *node, char c) {
	Node new_node = init_node();
	Edge e = (Edge) {c, &new_node, 0};
	node->edges = (Edge*)realloc(node->edges, sizeof(Edge) * (node->E + 1));
	if (!node->edges) {
		perror("Can't allocate memory");
		return NULL;
	}
	
	(node->edges)[node->E++] = e;
	return node;
}

void count_traverse(const Node *trie, char *buf);

void add_to_trie(Node *t, const char *word) {
	int i;
	for (i = 0; i < t->E; ++i) {
		if (t->edges[i].c == *word) 
			break;
	}

	printf("node: %p; edge count: %d\n", t, t->E);
	printf("adding word %s\n", word);
	//count_traverse(&trie);
	if (i == t->E) // не нашлось ребро с *word
		add_edge(t, *word); 

	
	if (*(word + 1) == 0)
		t->edges[i].count++;
	else
		add_to_trie(t->edges[i].next, word + 1);
}

void count_traverse(const Node *trie, char *buf) {
	printf("\ncount_traverse(%p, %s)\n", trie, buf);
	printf("------------> {edges = %p, E = %d}\n", trie->edges, trie->E);

	if (!trie)
		return;
	if (!trie->edges)
		return;
	int i;
	int L = strlen(buf);
	for (i = 0; i < trie->E; ++i) {
		if (!trie->edges[i].next)
			continue;
		buf[L] = trie->edges[i].c;
		if (trie->edges[i].count != 0) { 
			printf("Traverse: %s : %d times, next is: %p\n", buf, trie->edges[i].count, trie->edges[i].next);
		}
		count_traverse(trie->edges[i].next, buf);
	}
	buf[L] = 0;
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

void to_downcase(char *w) {
	while (*w) {
		*w = tolower(*w);
		w++;
	}
}

int main() {
	freopen("in.txt", "r", stdin);
	char word[MAX_WORD_LEN];

	/*WordInfo infos[MAX_WORDS];

	
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

	// Node trie;
	trie = init_node();

	char buf[MAX_WORD_LEN]; // для обхода дерева
	memset(buf, 0, sizeof(char) * MAX_WORD_LEN);

	/* Строим бор */
	while (scanf("%s", word) != EOF) {
		char *trimmed = trim(word);
		to_downcase(trimmed);
		add_to_trie(&trie, trimmed);
		count_traverse(&trie, buf);
		add_to_trie(&trie, trimmed);
	}

	memset(buf, 0, sizeof(char) * MAX_WORD_LEN);

	count_traverse(&trie, buf);

	return 0;
}

