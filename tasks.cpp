#include "source/task.h"
#include "source/DSL.h"

Elem_t treeGetValue(Tree *head, Vocabulary *varList, int *err) {
    catchNullptr(head, POISON);

    Elem_t val = treeGetNodeValue(head -> tree, varList, err);
    if (*err) return POISON;

    return val;
}

int treePrintNTeylor(Tree *head, Vocabulary *varList, size_t n, FILE *stream) {
    catchNullptr(  head , TreeIsNull);
    catchNullptr( stream, TreeFileOutErr);
    catchNullptr(varList, TreeIsNull);

    Tree dif = {};

    int err  = treeCtor(&dif);
    dif.tree = treeDifferential(head -> tree, varList, &err);
    err |= treeSimplify(&(dif.tree));
    err |= setVarValue(varList, "x", 0);
    fprintf(stderr, "%d!\n", err);
    if (err) return err;

    treeGraphVizDump(&dif, "logs/graph.dot", 10);

    Elem_t val = treeGetNodeValue(dif.tree, varList, &err);
    if (err) return err;

    fprintf(stream, "%lg ", val);

    for (size_t cur = 1; cur <= n; cur++) {
        dif.tree = treeDifferential(dif.tree, varList, &err);
        if (err) return err;

        val = treeGetNodeValue(dif.tree, varList, &err);
        if (err) return err;

        if (val)
            fprintf(stream, "+ \\frac{%lg \\cdot x^%zu}{%zu!} ", val, cur, cur);
        else break;
    }
    fprintf(stream, "+ o(x^%zu)", n);

    return TreeIsOk;
}

Elem_t treeGetNodeValue(TreeNode *node, Vocabulary *varList, int *err) {
    catchNullptr(node, POISON);

    Elem_t val1 = 0;
    Elem_t val2 = 0;

    if (nType(node) == Numeral)
        return nNum(node);

    if (nType(node) == Varriable) 
        return getVarValue(varList, nVar(node));

    if (L(node) != nullptr) 
        val1 = treeGetNodeValue( L(node), varList, err);
    if (R(node) != nullptr)
        val2 = treeGetNodeValue(R(node), varList, err);
    
    if (val1 == POISON || val2 == POISON || *err) {
        *err |= TreeDataPoison;
        return POISON;
    }

    switch(nOp(node)) {
        case Add: return val1 + val2;
        case Sub: return val1 - val2;
        case Mul: return val1 * val2;
        case Div:
            if (val2 == 0) {
                *err = TreeDevisionByZero;
                return POISON;
            }
            return val1 / val2;

        case Pow: return pow(val1, val2);
        case Cos: return cos(val2);
        case Sin: return sin(val2);
        case Log: return log(val2);
    }

    return POISON;
}