#include "source/tree.h"
#include "source/latex.h"
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

    node -> nodeName =  nullptr;

    node ->   lft    =  nullptr;
    node ->   rgt    =  nullptr;
    node ->   size   =     1   ;

    node ->   type   = nodeType;
    node -> data.var = nodeData;

    node ->  status  =  Active ;

    return TreeIsOk;
}

int treeNumNodeCtor(TreeNode *node, enum NodeType nodeType, Elem_t nodeData) {
    catchNullptr(node, TreeIsNull);

    node -> nodeName =  nullptr;

    node ->   lft    =  nullptr;
    node ->   rgt    =  nullptr;
    node ->   size   =     1   ;

    node ->   type   = nodeType;
    node ->  data.num = nodeData;

    node ->  status  =  Active ;

    return TreeIsOk;
}

int treeOpNodeCtor(TreeNode *node, enum NodeType nodeType, char nodeData) {
    catchNullptr(node, TreeIsNull);

    node -> nodeName =  nullptr;

    node ->   lft    =  nullptr;
    node ->   rgt    =  nullptr;
    node ->   size   =     1   ;

    node ->   type   = nodeType;
    node ->  data.op = nodeData;

    node ->  status  =  Active ;

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

    if (R(node) != nullptr) 
        err |= treeNodeDtor(R(node));

    if (L(node) != nullptr)
        err |= treeNodeDtor(L(node));

    free(node);
    
    return err;
}

TreeNode* treeDifferential(TreeNode *node, Vocabulary *varList, int *err, FILE *stream) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;

    switch (nType(node)) {
        case Numeral:
            *err |= newNumNode(&result, Numeral, 0);
            if (*err) return nullptr;

            break;
        
        case Varriable: 
            *err |= newNumNode(&result, Numeral, 1);
            if (*err) return nullptr;

            break;

        case Operand:
            switch(nOp(node)) {
                case Add: 
                    result = ADD(dL, dR);
                    break;
                case Sub: 
                    result = SUB(dL, dR);
                    break;
                case Mul: 
                    result = ADD(MUL(dL, cR), MUL(cL, dR));
                    break;
                case Div: 
                    result = DIV(SUB(MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));
                    break;
                case Sin: 
                    result = MUL(COS(cR), dR);
                    break;
                case Cos: 
                    result = MUL(NEG(SIN(cR)), dR);
                    break;
                case Log: 
                    result = MUL(REV(cR), dR);
                    break;
                case Pow: 
                    result = diffPow(node, varList, err, stream);
                    break;

                default:
                    result = nullptr;
                    break;
            }
        
        default: break;
    }
    // updateNode(result, varList);

    if (result != nullptr && stream != nullptr) {
        updateNode(node, varList);
        fprintf(stream, "%s$$(", bundles[rand() % 20]);
        *err |= treePrintNode(node, varList, stream, 1);
        fprintf(stream, ")^\\prime = ");
        *err |= treePrintNode(result, varList, stream, 1);
        fprintf(stream, "$$\n");
        // fprintf(stderr, "%d - %d!\n", curDes, DESES.usedDes);
        if (DESES.usedDes) {
            fprintf(stream, "Где ");
            for (size_t curDes = 0; curDes < DESES.usedDes; curDes++) {
                fprintf(stream, "$%s = ", DESES.desig[curDes]);
                *err |= treePrintNode(DESES.nodes[curDes], varList, stream, 0);
                // fprintf(stderr, "HERE %d!\n", DESES.usedDes);
                fprintf(stream, "$,\\; \n");
            }
            fprintf(stream, "\\\\\\\\\\\\\n");
        }

        DESES.usedDes = 0;
    }
    
    return result;
}

TreeNode* treeCopy(TreeNode *node, Vocabulary *varList, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;

    if (nType(node) == Operand) {
        switch(nOp(node)) {
            case Add: 
                result = ADD(cL, cR);
                break;                
            case Sub: 
                result = SUB(cL, cR);
                break;                
            case Mul: 
                result = MUL(cL, cR);
                break;                
            case Div: 
                result = DIV(cL, cR);
                break;                
            case Pow: 
                result = POW(cL, cR);
                break;                
            case Cos: 
                result = COS(cR);
                break;                
            case Sin: 
                result = SIN(cR);
                break;                
            case Log: 
                result = LOG(cR);
                break;                

            default:  
                result = nullptr;
                break;
        }

        // updateNode(result, varList);

        return result;
    }

    if (nType(node) == Numeral)
        *err |= newNumNode(&result, nType(node), nNum(node));
    else if (nType(node) == Varriable)
        *err |= newVarNode(&result, nType(node), nVar(node));
    else
        *err = TreeTypeIsNone;

    if (*err) return nullptr;

    // updateNode(result, varList);

    return result;
}

TreeNode *treeDoubleArgumentOper(TreeNode *node1, TreeNode *node2, enum OperandType type, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, type);
    if (*err) return nullptr;

    L(result) = node1;
    R(result) = node2;

    return result;
}

TreeNode *treeOneArgumentOper(TreeNode *node, enum OperandType type, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, type);
    if (*err) return nullptr;

       
    //----------------------
    R(result) = node;
    //----------------------

    return result;
}

TreeNode *treeNeg(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err |= newOpNode(&result, Operand, Mul);
    *err |= newNumNode(&(L(result)), Numeral, -1);
    if (*err) return nullptr;

    R(result) = node;

    return result;
}

TreeNode *treeRev(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;
    *err |= newOpNode(&result, Operand, Div);
    *err |= newNumNode(&(L(result)), Numeral, 1);
    if (*err) return nullptr;

    R(result) = node;

    return result;
}

TreeNode* diffPow(TreeNode *node, Vocabulary *varList, int *err, FILE *stream) {
    catchNullptr(node, nullptr);

    TreeNode  *result  =          nullptr            ;
    TreeNode *varLeft  = treeFindVarriable(L(node) , err);
    TreeNode *varRight = treeFindVarriable(R(node), err);

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

int treeSimplify(TreeNode **node) {
    catchNullptr(node, TreeIsNull);

    if (nType(*node) == Numeral || nType(*node) == Varriable)
        return EXIT_SUCCESS;

    int err = EXIT_SUCCESS;


    if (L(*node) != nullptr)
        err |= treeSimplify(&(L(*node)));
    
    if (R(*node) != nullptr)
        err |= treeSimplify(&(R(*node)));

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

    if (R(*node) == nullptr)
        return EXIT_SUCCESS;

    int err = EXIT_SUCCESS;


    if (nType(L(*node)) == Numeral && nType(R(*node)) == Numeral) {
        Elem_t newData = 0;
        switch(type) {
            case Add:
                newData = nNum(L(*node)) + nNum(R(*node));
                break;
            
            case Sub:
                newData = nNum(L(*node)) - nNum(R(*node));
                break;
            
            case Mul:
                newData = nNum(L(*node)) * nNum(R(*node));
                break;
            
            case Div:
                newData = nNum(L(*node)) / nNum(R(*node));
                break;

            case Pow:
                newData = pow(nNum(L(*node)), nNum(R(*node)));
                break;
            
            default: return TreeUnknownOperand;
        }

        err |= treeNodeDtor(*node);
        err |= newNumNode(node, Numeral, newData);

        return err;
    }

    if (type == Pow)
        return treeMakeSimplePow(node);
    
    if (type == Mul)
        return treeMakeSimpleMul(node);

    if (type == Div)
        return treeMakeSimpleDiv(node);

    //Add or Sub
    double neutral = 0;

    if (nType(L(*node)) == Numeral && nNum(L(*node)) == neutral) 
        err |= treeMakeAncRgt(node);
    if (nType(R(*node)) == Numeral && nNum(R(*node)) == neutral)
        err |= treeMakeAncLft(node);
    //
    
    return err;
}

int treeMakeSimplePow(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);


    if (nType(R(*node)) == Numeral && nNum(R(*node)) == 1)
        return treeMakeAncLft(node);

    if (nType(R(*node)) == Numeral && nNum(R(*node)) == 0) {
        int err  = treeNodeDtor(*node);
            err |= newNumNode(node, Numeral, 1);

        return err;
    }

    if (nType(L(*node)) == Numeral && (nNum(L(*node)) == 1 || nNum(L(*node)) == 0)) 
        return treeMakeAncLft(node);
    
    return TreeIsOk;
}

int treeMakeSimpleMul(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);

    if (nType(R(*node)) == Numeral && nNum(R(*node)) == 1)
        return treeMakeAncLft(node);
    if (nType(L(*node)) == Numeral  && nNum(L(*node))  == 1)
        return treeMakeAncRgt(node);

    // fprintf(stderr, "here! %08X -> %d\n", nRight(*node), nType(nRight(*node)));
    if ((nType(R(*node)) == Numeral && nNum(R(*node)) == 0) || 
        (nType(L( *node)) == Numeral && nNum(L( *node)) == 0)) {
        int err  = treeNodeDtor(*node);
            err |= newNumNode(node, Numeral, 0);

        return err;
    }
    
    return TreeIsOk;
}

int treeMakeSimpleDiv(TreeNode **node) {
    catchNullptr( node, TreeIsNull);
    catchNullptr(*node, TreeIsNull);

    if (nType(R(*node)) == Numeral && nNum(R(*node)) == 0) return TreeDevisionByZero;

    if (nType(R(*node)) == Numeral && nNum(R(*node)) == 1)
        return treeMakeAncLft(node);

    if (nType(L(*node)) == Numeral  && nNum(L(*node)) == 0)
        return treeMakeAncLft(node);
    
    return TreeIsOk;
}

static int treeMakeAncRgt(TreeNode **node) {
    catchNullptr(    node     , TreeIsNull);
    catchNullptr(   *node     , TreeIsNull);
    catchNullptr(R(*node), TreeIsNull);

    int err = treeNodeDtor(L(*node));
    free(*node);
    *node = R(*node);

    return err;
}

static int treeMakeAncLft(TreeNode **node) {
    catchNullptr(    node    , TreeIsNull);
    catchNullptr(   *node    , TreeIsNull);
    catchNullptr(L(*node), TreeIsNull);

    int err = treeNodeDtor(R(*node));
    free(*node);
    *node = L(*node);

    return err;
} 

TreeNode* treeFindVarriable(TreeNode *node, int *err) {
    if (node == nullptr) return nullptr;
    
    TreeNode *lftRes = nullptr;
    TreeNode *rgtRes = nullptr;

    // fprintf(stderr, "%08X -> %d\n", node, node -> type);

    switch (nType(node)) {
        case Numeral:
            return nullptr;
        
        case Varriable:
            // fprintf(stderr, "!!%08X -> %d\n", node, node -> type);
            return node;

        case Operand:
             lftRes = treeFindVarriable(L(node), err);
             rgtRes = treeFindVarriable(R(node), err);

            // fprintf(stderr, "%08X - %08X\n", lftRes, rgtRes);

            if (lftRes == nullptr && rgtRes == nullptr)
                return nullptr;

            if (lftRes == nullptr)
                return rgtRes;

            
            return lftRes;
        
        default: return nullptr;
    }
}

int vocabularyDtor(Vocabulary *voc) {
    catchNullptr(voc, TreeIsNull);

    for (size_t cur = 0; cur < voc -> size; cur++)
        voc -> var[cur] = nullptr;
    voc -> size = 0;

    return TreeIsOk;
}

Elem_t getVarValue(Vocabulary *varList, const char *varName) {
    catchNullptr(varList, POISON);
    catchNullptr(varName, POISON);

    for (size_t cur = 0; cur < varList -> size; ++cur)
        if (!strcmp(varName, varList -> var[cur]))
            return varList -> value[cur];

    return POISON;
}

int setVarValue(Vocabulary *varList, const char *varName, Elem_t value) {
    catchNullptr(varList, EXIT_FAILURE);
    catchNullptr(varName, EXIT_FAILURE);

    fprintf(stderr, "%d", varList -> size);
    for (size_t cur = 0; cur < varList -> size; ++cur)
        if (!strcmp(varName, varList -> var[cur])) {
            varList -> value[cur] = value;
            return EXIT_SUCCESS;
        }

    return EXIT_FAILURE;
}

void updateNode(TreeNode *node, Vocabulary *varList) {
    if (node == nullptr) return;

    updateNode( L(node), varList);
    updateNode(R(node), varList);

    if ( L(node) != nullptr) node -> size +=  L(node) -> size;
    if (R(node) != nullptr) node -> size += R(node) -> size;

    if (node -> size > MAX_NODE_SIZE && DESES.usedDes < DESIGNATIONS_SIZE && node -> nodeName == nullptr) {
        DESES.nodes[DESES.usedDes] = node;
        node -> nodeName = DESES.desig[DESES.usedDes++];
        node ->   size   =             1               ;
        // fprintf(stderr, "%08X\n", node);
    }
}

// void vizLogClose() {
//     fclose(LogGraph);
// }