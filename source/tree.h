// #include "stack.h"
#include "treeType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum TreeErrors {
    TreeIsOk                 =       0,
    TreeIsNull               = 1 <<  0,
    TreeFileInErr            = 1 <<  1,
    TreeIsInActive           = 1 <<  2,
    TreeTypeIsNone           = 1 <<  3,
    TreeFileOutErr           = 1 <<  4,
    TreeDivisionByZero       = 1 <<  5,
    TreeUnknownOperand       = 1 <<  6,
    TreeDoubleDestruction    = 1 <<  7,
    TreeDoubleConstruction   = 1 <<  8,
    TreeGraphVizExecutionErr = 1 <<  9,
};

enum TreeStatus {
     Active  = 0,
    InActive = 1,
};

enum Result {
    NotFound = 0,
     Found   = 1, 
};


struct TreeInfo {
    size_t  line   =    0   ;

    char *  name   = nullptr;
    char *  file   = nullptr;
    char * pointer = nullptr;
    char *function = nullptr;
};

struct TreeNode {

    enum NodeType type = None;

    union NodeData {
        char   *var  ;
        char    op   ;
        double  num  ;
    } data;


    TreeStatus  status = InActive;

    TreeNode    *rgt   =  nullptr;
    TreeNode    *lft   =  nullptr;
};


struct Tree {
    TreeNode   *tree   =    {}   ;
    TreeInfo    info   =    {}   ;
     size_t     size   =    0    ;
    TreeStatus status  = InActive;
};

#define treeCtor(TREE) _treeCtor((TREE), #TREE, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define treeDump(TREE) treeDump_((TREE), __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define catchNullptr(POINTER, RETURN_VALUE) {                                                                      \
    if ((POINTER) == nullptr) {                                                                                     \
        fprintf(stderr, "%s pointer at %s at %s(%d) is NULL\n", #POINTER, __PRETTY_FUNCTION__, __FILE__, __LINE__);  \
        return RETURN_VALUE;                                                                                          \
    }                                                                                                                  \
}

int newVarNode(TreeNode **node, enum NodeType type, char  *nodeData);

int newNumNode(TreeNode **node, enum NodeType type, double nodeData);

int newOpNode (TreeNode **node, enum NodeType type, char   nodeData);

int treeVarNodeCtor(TreeNode *node, enum NodeType nodeType, char  *nodeData);

int treeNumNodeCtor(TreeNode *node, enum NodeType nodeType, double nodeData);

int treeOpNodeCtor (TreeNode *node, enum NodeType nodeType, char   nodeData);

int _treeCtor(Tree *root, const char * name, const char *file, const char *function, size_t line);

int treeHeadDtor(Tree *root);

int treeNodeDtor(TreeNode *node);

int treeLoadBase(Tree *head, const char *fileName);

int treeBaseScanf(TreeNode **node, char **buffer);

int treeMakeSimple(TreeNode **node);

int treeMakeSimpleOperand(TreeNode **node, enum OperandType type);

int treeMakeSimplePow(TreeNode **node);

int treeMakeSimpleMul(TreeNode **node);

int treeMakeSimpleDiv(TreeNode **node);

TreeNode* treeDifferential(TreeNode *node, int *err);

TreeNode* treeCopy(TreeNode *node, int *err);

TreeNode* diffPow(TreeNode *node, int *err);

TreeNode *treeDoubleArgumentOper(TreeNode *node1, TreeNode *node2, enum OperandType type, int *err);

TreeNode *treeOneArgumentOper(TreeNode *node, enum OperandType type, int *err);

TreeNode *treeNeg(TreeNode *node, int *err);

TreeNode *treeRev(TreeNode *node, int *err);

int treePrintEquat(Tree *head, const char *inFileName);

int treePrintNode(TreeNode *node, FILE *stream);

const char *treeGraphVizDump(Tree *tree, const char *fileName, int cmd);

void treePrintNodeGrVz(TreeNode *tree, size_t *cur, FILE *stream);

void treePrintErrorMessage(int error);

int treeNew(TreeNode **node, char *curCmd);

TreeNode* treeFindVarriable(TreeNode *node, int *err);