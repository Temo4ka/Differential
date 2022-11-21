#include "source/tree.h"
#include "source/config.h"
#include "source/DSL.h"
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

int newVarNode(TreeNode **node, enum NodeType type, char  *nodeData) {
    *node = (TreeNode *) calloc(1, sizeof(TreeNode));
    return treeVarNodeCtor(*node, type, nodeData);
}

int newNumNode(TreeNode **node, enum NodeType type, double nodeData) {
    *node = (TreeNode *) calloc(1, sizeof(TreeNode));
    return treeNumNodeCtor(*node, type, nodeData);
}

int newOpNode (TreeNode **node, enum NodeType type, char   nodeData) {
    *node = (TreeNode *) calloc(1, sizeof(TreeNode));
    return treeOpNodeCtor(*node, type, nodeData);
}

int treeVarNodeCtor(TreeNode *node, enum NodeType nodeType, char *nodeData) {
    catchNullptr(node, TreeIsNull);

    node ->  rgt   =  nullptr;
    node ->  lft   =  nullptr;

    node ->     type      = nodeType;
    node ->  data.var = nodeData;

    node -> status =  Active ;

    return TreeIsOk;
}

int treeNumNodeCtor(TreeNode *node, enum NodeType nodeType, double nodeData) {
    catchNullptr(node, TreeIsNull);

    node ->    rgt    =  nullptr;
    node ->    lft    =  nullptr;

    node ->    type   = nodeType;
    node ->  data.num = nodeData;

    node ->  status   =  Active ;

    return TreeIsOk;
}

int treeOpNodeCtor(TreeNode *node, enum NodeType nodeType, char nodeData) {
    catchNullptr(node, TreeIsNull);

    node ->  rgt   =  nullptr;
    node ->  lft   =  nullptr;

    node ->   type   = nodeType;
    node ->  data.op = nodeData;

    node -> status =  Active ;

    return TreeIsOk;
}


int _treeCtor(Tree *root, const char * name, const char *file, const char *function, size_t line) {
    catchNullptr(root, TreeIsNull);
    if (root -> status == Active) return TreeDoubleConstruction;

    root -> info. pointer = (char *)   root  ;
    root -> info.  name   = (char *)   name  ;
    root -> info.  file   = (char *)   file  ;
    root -> info.function = (char *) function;
    root -> info.  line   =        line      ;
    root ->     tree      =       nullptr    ;

    root ->     status    =      Active      ;

    return TreeIsOk;
}

int treeHeadDtor(Tree *root) {
    catchNullptr(root, TreeIsNull);

    if (root -> status == InActive) return TreeDoubleDestruction;

    if (treeNodeDtor(root -> tree)) return EXIT_FAILURE;

    free(root -> tree);

    root -> status = InActive;

    return TreeIsOk;

}

int treeNodeDtor(TreeNode *node) {
    if (node == nullptr)
        return TreeIsOk;

    int err = TreeIsOk;

    if (nRight(node) != nullptr) 
        err |= treeNodeDtor(nRight(node));

    if (nLeft(node) != nullptr)
        err |= treeNodeDtor(nLeft(node));

    free(node);
    
    return err;
}

TreeNode* treeDifferential(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;


    switch (nType(node)) {
        case Numeral:
            *err |= newNumNode(&result, Numeral, 0);
            if (*err) return nullptr;

            return result;
        
        case Varriable: 
            *err |= newNumNode(&result, Numeral, 1);
            if (*err) return nullptr;

            return result;

        case Operand:
            switch(nOp(node)) {
                case Add: return ADD(dL, dR);
                case Sub: return SUB(dL, dR);
                case Mul: return ADD(MUL(dL, cR), MUL(cL, dR));
                case Div: return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));
                case Sin: return MUL(COS(cR), dR);
                case Cos: return MUL(NEG(SIN(cR)), dR);
                case Log: return MUL(REV(cR), dR);
                case Pow: return diffPow(node, err);

                default: return nullptr;
            }
        
        default: return nullptr;
    }
}

TreeNode* treeCopy(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    if (nType(node) == Operand) {
        switch(nOp(node)) {
            case Add: return ADD(cL, cR);
            case Sub: return SUB(cL, cR);
            case Mul: return MUL(cL, cR);
            case Div: return DIV(cL, cR);
            case Pow: return POW(cL, cR);
            case Cos: return COS(cR);
            case Sin: return SIN(cR);
            case Log: return LOG(cR);

            default: return nullptr;
        }
    }

    TreeNode *result = nullptr;

    if (nType(node) == Numeral)
        *err |= newNumNode(&result, nType(node), nNum(node));
    else if (nType(node) == Varriable)
        *err |= newVarNode(&result, nType(node), nVar(node));
    else
        *err = TreeTypeIsNone;

    if (*err) return nullptr;

    return result;
}

TreeNode *treeDoubleArgumentOper(TreeNode *node1, TreeNode *node2, enum OperandType type, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, type);
    if (*err) return nullptr;

    nLeft(result) = node1;
    nRight(result) = node2;

    return result;
}

TreeNode *treeOneArgumentOper(TreeNode *node, enum OperandType type, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, type);
    if (*err) return nullptr;

    nRight(result) = node;

    return result;
}

TreeNode *treeNeg(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err |= newOpNode(&result, Operand, Mul);
    *err |= newNumNode(&(nLeft(result)), Numeral, -1);
    if (*err) return nullptr;

    nRight(result) = node;

    return result;
}

TreeNode *treeRev(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err |= newOpNode(&result, Operand, Div);
    *err |= newNumNode(&(nLeft(result)), Numeral, 1);
    if (*err) return nullptr;

    nRight(result) = node;

    return result;
}

TreeNode* diffPow(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode  *result  =          nullptr            ;
    TreeNode *varLeft  = treeFindVarriable(nLeft(node) , err);
    TreeNode *varRight = treeFindVarriable(nRight(node), err);

    if (varLeft != nullptr && varRight != nullptr) {
        TreeNode *one = nullptr;
        *err = newNumNode(&one, Numeral, 1);

        return ADD(MUL(dL, d_aX), MUL(dR, d_xA));
    }
    if (varLeft == nullptr)
        return d_aX;
    if (varRight == nullptr) {
        TreeNode *one = nullptr;
        *err = newNumNode(&one, Numeral, 1);

        return d_xA;
    }

    *err = newNumNode(&result, Numeral, 0);

    return result;
}

int treeMakeSimple(TreeNode **node) {
    catchNullptr(node, TreeIsNull);

    if (nType(*node) == Numeral || nType(*node) == Varriable)
        return EXIT_SUCCESS;

    int err = EXIT_SUCCESS;

    if (nLeft(*node) != nullptr)
        err |= treeMakeSimple(&(nLeft(*node)));
    
    if (nRight(*node) != nullptr)
        err |= treeMakeSimple(&(nRight(*node)));

    if (err) return err;


    switch(nOp(*node)) {
        case Add:
            err |= treeMakeSimpleOperand(node, Add);
            break;
        
        case Sub:
            err |= treeMakeSimpleOperand(node, Sub);
            break;

        case Mul:
            err |= treeMakeSimpleOperand(node, Mul);
            break;
        
        case Div:
            err |= treeMakeSimpleOperand(node, Div);
            break;
        
        case Pow:
            err |= treeMakeSimpleOperand(node, Pow);
            break;
        
        default: break;
    }

    return err;
}

static treeMakeAncRgt(TreeNode **node);

static treeMakeAncLft(TreeNode **node);

int treeMakeSimpleOperand(TreeNode **node, enum OperandType type) {
    catchNullptr(node, TreeIsNull);

    if (nLeft(*node) == nullptr || nRight(*node) == nullptr)
        return EXIT_SUCCESS;

    int err = EXIT_SUCCESS;

    if (nType(nLeft(*node)) == Numeral && nType(nRight(*node)) == Numeral) {
        double newData = 0;
        switch(type) {
            case Add:
                newData = nNum(nLeft(*node)) + nNum(nRight(*node));
                break;
            
            case Sub:
                newData = nNum(nLeft(*node)) - nNum(nRight(*node));
                break;
            
            case Mul:
                newData = nNum(nLeft(*node)) * nNum(nRight(*node));
                break;
            
            case Div:
                newData = nNum(nLeft(*node)) / nNum(nRight(*node));
                break;

            case Pow:
                newData = pow(nNum(nLeft(*node)), nNum(nRight(*node)));
                break;
            
            default: return TreeUnknownOperand;
        }

        err |= treeNodeDtor(*node);
        err |= newNumNode(node, Numeral, newData);
    }

    if (type == Pow)
        return treeMakeSimplePow(node);
    
    if (type == Mul)
        return treeMakeSimpleMul(node);

    if (type == Div)
        return treeMakeSimpleDiv(node);

    //Add or Sub
    double neutral = 0;

    if (nType(nLeft(*node)) == Numeral && nNum(nLeft(*node)) == neutral) 
        err |= treeMakeAncRgt(node);
    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == neutral)
        err |= treeMakeAncLft(node);
    //
    
    return err;
}

int treeMakeSimplePow(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);

    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == 1)
        return treeMakeAncLft(node);

    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == 0) {
        // fprintf(stderr, "here!\n");
        int err  = treeNodeDtor(*node);
            err |= newNumNode(node, Numeral, 1);

        return err;
    }

    if (nType(nLeft(*node)) == Numeral && (nNum(nLeft(*node)) == 1 || nNum(nLeft(*node)) == 0))
        return treeMakeAncLft(node);
    
    return TreeIsOk;
}

int treeMakeSimpleMul(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);

    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == 1)
        return treeMakeAncLft(node);
    if (nType(nLeft(*node)) == Numeral  && nNum(nLeft(*node))  == 1)
        return treeMakeAncRgt(node);

    if (nType(nRight(*node)) == Numeral && (nNum(nRight(*node)) == 0 || nNum(nLeft(*node)) == 0)) {
        int err  = treeNodeDtor(*node);
            err |= newNumNode(node, Numeral, 0);

        return err;
    }
    
    return TreeIsOk;
}

int treeMakeSimpleDiv(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);

    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == 0) return TreeDivisionByZero;

    if (nType(nRight(*node)) == Numeral && nNum(nRight(*node)) == 1)
        return treeMakeAncLft(node);

    if (nType(nLeft(*node)) == Numeral  && nNum(nLeft(*node)) == 0)
        return treeMakeAncLft(node);
    
    return TreeIsOk;
}

static int treeMakeAncRgt(TreeNode **node) {
    catchNullptr(    node     , TreeIsNull);
    catchNullptr(   *node     , TreeIsNull);
    catchNullptr(nRight(*node), TreeIsNull);

    int err = treeNodeDtor(nLeft(*node));
    free(*node);
    *node = nRight(*node);

    return err;
}

static int treeMakeAncLft(TreeNode **node) {
    catchNullptr(    node    , TreeIsNull);
    catchNullptr(   *node    , TreeIsNull);
    catchNullptr(nLeft(*node), TreeIsNull);

    int err = treeNodeDtor(nRight(*node));
    free(*node);
    *node = nLeft(*node);

    return err;
} 

TreeNode* treeFindVarriable(TreeNode *node, int *err) {
    if (node == nullptr) return nullptr;
    
    TreeNode *lftRes = nullptr;
    TreeNode *rgtRes = nullptr;

    switch (nType(node)) {
        case Numeral:
            return nullptr;
        
        case Varriable:
            return node;

        case Operand:
             lftRes = treeFindVarriable(nLeft(node), err);
             rgtRes = treeFindVarriable(nLeft(node), err);

            if (lftRes == nullptr && rgtRes == nullptr);
                return nullptr;
            if (lftRes == nullptr)
                return rgtRes;
            
            return lftRes;
        
        default: return nullptr;
    }
}

// void vizLogClose() {
//     fclose(LogGraph);
// }