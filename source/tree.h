// #include "stack.h"
#include "treeType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"

enum TreeErrors {
    TreeIsOk                 =       0,
    TreeIsNull               = 1 <<  0,
    TreeFileInErr            = 1 <<  1,
    TreeFileOutErr           = 1 <<  2,
    TreeDataPoison           = 1 <<  3,
    TreeIsInActive           = 1 <<  4,
    TreeTypeIsNone           = 1 <<  5,
    TreeDevisionByZero       = 1 <<  6,
    TreeUnknownOperand       = 1 <<  7,
    TreeDoubleDestruction    = 1 <<  8,
    TreeDoubleConstruction   = 1 <<  9,
    TreeGraphVizExecutionErr = 1 << 10,
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
    const char *nodeName = nullptr;

    enum NodeType type = None;

    union NodeData {
        char   *var  ;
        char    op   ;
        double  num  ;
    } data;


    TreeStatus  status = InActive;

    TreeNode    *rgt   =  nullptr;
    TreeNode    *lft   =  nullptr;

    size_t      size   =     1   ;
};


struct Tree {
    TreeNode   *tree   =    {}   ;
    TreeInfo    info   =    {}   ;
     size_t     size   =    0    ;
    TreeStatus status  = InActive;
};

struct NodesDes {
    size_t usedDes = 0;

    TreeNode *nodes[DESIGNATIONS_SIZE] = {};

    const   char   *desig[DESIGNATIONS_SIZE] = {
        "\\tau",
        "\\alpha",
        "\\beta",
        "\\gamma",
        "\\Delta",
        "\\xi",
        "\\sigma",
        "\\varphi",
        "\\psi",
        "\\omega"
        "\\varepsilon",
        "\\zeta",
        "\\eta",
        "\\theta",
        "\\iota",
        "\\varkappa",
        "\\lambda",
        "\\mu",
        "\\nu",
        "\\upsilon"
    };
};

struct Vocabulary {
    NodesDes    designations     = {};
      char   *var[MAX_VOC_SIZE]  = {};
     Elem_t  value[MAX_VOC_SIZE] = {};
     size_t        size          =  0;
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

int vocabularyDtor(Vocabulary *voc);

int treeLoadBase(Tree *head, const char *fileName, Vocabulary *varList);

int treeBaseScanf(TreeNode **node, char **buffer, Vocabulary *varList);

int treeSimplify(TreeNode **node);

int treeMakeSimpleOperand(TreeNode **node, enum OperandType type);

int treeMakeSimplePow(TreeNode **node);

int treeMakeSimpleMul(TreeNode **node);

int treeMakeSimpleDiv(TreeNode **node);

TreeNode* treeDifferential(TreeNode *node, Vocabulary *varList, int *err, FILE *stream = nullptr);

TreeNode* treeCopy(TreeNode *node, Vocabulary *varList, int *err);

TreeNode* diffPow(TreeNode *node, Vocabulary *varList, int *err, FILE *stream = nullptr);

TreeNode *treeDoubleArgumentOper(TreeNode *node1, TreeNode *node2, enum OperandType type, int *err);

TreeNode *treeOneArgumentOper(TreeNode *node, enum OperandType type, int *err);

TreeNode *treeNeg(TreeNode *node, int *err);

TreeNode *treeRev(TreeNode *node, int *err);

int treePrintEquat(Tree *head, Vocabulary *varList, const char *inFileName);

int treePrintNode(TreeNode *node, Vocabulary *varList, FILE *stream, bool cut = 0);

const char *treeGraphVizDump(Tree *tree, const char *fileName, int cmd);

void treePrintNodeGrVz(TreeNode *tree, size_t *cur, FILE *stream);

void treePrintErrorMessage(int error);

int treeNew(TreeNode **node, char *curCmd);

TreeNode* treeFindVarriable(TreeNode *node, int *err);

Elem_t getVarValue(Vocabulary *varList, const char *varName);

int setVarValue(Vocabulary *varList, const char *varName, Elem_t value);

void updateNode(TreeNode *node, Vocabulary *varList);