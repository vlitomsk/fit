#ifndef AVLTREE_H_INCLUDED
#define AVLTREE_H_INCLUDED

typedef struct Node_s {
	int key;
	unsigned h;
	struct Node_s *l, *r;
} Node;

typedef enum {false, true} bool;

extern int height(const Node *p);
extern Node *insert(Node *p, int key);
extern void freeTree(Node *p);
extern void dfs(Node *p);

#endif
