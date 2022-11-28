#define LATEX 1
#include "tree.h"
#include "latex.h"

Elem_t treeGetValue(Tree *head, Vocabulary *varList, int *err);

int treePrintNTeylor(Tree *head, Vocabulary *varList, size_t n, FILE *stream);

Elem_t treeGetNodeValue(TreeNode *node, Vocabulary *varList, int *err);