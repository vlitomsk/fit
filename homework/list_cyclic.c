#include <stdio.h>
#include <stdlib.h>

/* max. node count */
#define MAX_NODES 100

typedef struct Node_s {
  int v;   /* head */
  struct Node_s *n; /* tail */
} Node;
typedef Node* List;

/* builds list from raw input */
List build_list(const int *raw_values, const int* raw_ptrs, const int n, Node* nodes) {
  if (n == 0)
      return NULL;

  int i;
  for (i = 0; i < n; ++i)
    nodes[i] = (Node){raw_values[i], NULL};

  /* setting pointers in list */
  for (i = 0; i < n; ++i) {
    if (raw_ptrs[i] >= n)
      return NULL; // pointer to nonexistent node 
    if (raw_ptrs[i] >= 0)
      nodes[i].n = &nodes[raw_ptrs[i]];
  }

  return nodes;
}

Node* step(const Node *from, const int steps) {
  return (steps == 0 || !from) ? from : step(from->n, steps - 1);
}

/* checks if list is cyclic */
typedef enum {false, true} bool;
bool list_cyclic(const List lst) {
  Node *s;
  Node *f1;
  Node *f2;
  s = f1 = f2 = lst; // slow and fast pointers

  do {
    s = step(s, 1);
  } while (s && s != (f1 = step(f1, 2)) && s != (f2 = step(f2, 3)));

  return (s == f1 && f1) || (s == f2 && f2);
}

int main() {
  freopen("in2", "r", stdin);

  int n, raw_values[MAX_NODES], raw_ptrs[MAX_NODES];
  for (n = 0; n < MAX_NODES; ++n)
    if (scanf("%d %d", raw_values + n, raw_ptrs + n) == EOF)
      break;

  Node nodes[MAX_NODES]; // list data  
  List lst = build_list(raw_values, raw_ptrs, n, nodes);

  if (lst)
    printf("%d\n", list_cyclic(lst));
  else {
    fputs("List building failed!\n", stderr);
    return 1;
  }

  return 0;
}
