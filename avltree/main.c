#include "avltree.h"
#include <stdio.h>

int main(int argc, char **argv) {
	int N, i, val;
	scanf("%d\n", &N);
	Node *tree = NULL;
	for (i = 0; i < N; ++i) {
		scanf("%d", &val);
		tree = insert(tree, val);
	}
	printf("%d\n", tree ? height(tree)+1 : 0);
	freeTree(tree);
	
	return 0;
}
