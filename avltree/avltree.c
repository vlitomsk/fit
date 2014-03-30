#include "avltree.h"
#include <stdlib.h>
#include <assert.h>

int height(const Node *p) {
	return p ? p->h : 0; 
}

unsigned balanceFactor(const Node *p) {
	return height(p->r) - height(p->l);
}

unsigned fixHeight(Node *p) {
	unsigned hl = height(p->l), 
			 hr = height(p->r);

	p->h = hl > hr ? hl + 1 : hr + 1;
	return p->h;	
}

inline bool isLeaf(const Node *p) {
	return (!p->l && !p->r);
}

Node *singleNode(int key) {
	Node *p = (Node*)calloc(1, sizeof(Node));
	assert(p);
	p->key = key;
	return p;
}

/*
     p                  r
  l     r    =>     p      rr
      rl rr       l  rl
*/
Node *rotLeft(Node *p) {
	Node *tmp = p->r;
	p->r = p->r->l;  
	tmp->l = p;
	fixHeight(p);
	fixHeight(tmp);
	return tmp;   
}

/*
     p                 l 
  l     r    =>     ll    p           
ll lr                   lr  r         
*/
Node *rotRight(Node *p) {
	Node *tmp = p->l;
	p->l = p->l->r;
	tmp->r = p;
	fixHeight(p);
	fixHeight(tmp);
	return tmp;
}

Node *balance(Node *p) {
	fixHeight(p);
	if (balanceFactor(p) == 2) { // ������ ������� �������
		if (balanceFactor(p->r) < 0) { // ������ ����� ����� �������, ������� �������
			p->r = rotRight(p->r);
		} 
		p = rotLeft(p);
	} else if (balanceFactor(p) == -2) { // ����� �������
		if (balanceFactor(p->l) > 0) { // ����� ������ �������, ������� �������
			p->l = rotLeft(p->l);
		}
		p = rotRight(p);
	}
	
	return p;
}

Node *insert(Node *p, int key) {
	if (!p)
		return singleNode(key);
		
	if (key < p->key)
		p->l = insert(p->l, key);
	else
		p->r = insert(p->r, key);
		
	return balance(p);
}

void freeTree(Node *p) {
	if (!p)
		return;
	freeTree(p->l);
	freeTree(p->r);
	free(p);
}

