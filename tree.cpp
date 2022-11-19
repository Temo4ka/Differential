#include "source/tree.h"
#include "source/config.h"
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

static FILE *LogFile  = fopen("logs/logFile.txt", "w");

#define CUR_SYMB (*CUR_STR)

#define NEXT_SYMB CUR_STR++

#define CUR_STR (*buffer)

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

    if (treeDtor(root -> tree)) return EXIT_FAILURE;

    free(root -> tree);

    root -> status = InActive;

    return TreeIsOk;

}

int treeDtor(TreeNode *node) {
    if (node == nullptr)
        return TreeIsOk;

    int err = TreeIsOk;

    if (node -> rgt != nullptr) {
        err |= treeDtor(node -> rgt);
        free(node -> rgt);
    }
    if (node -> lft != nullptr) {
        err |= treeDtor(node -> lft);
        free(node -> lft);
    }
    
    return err;
}


static void printElem(FILE *stream, const Elem_t stackElement = nullptr) {
    fprintf(stream, "%s", stackElement);
    fprintf(stderr, "%s", stackElement);
}


static assignBuffer(char **buffer, const char *fileName);

static size_t getFileSize(const char *fileName);

int treeLoadBase(Tree *head, const char *fileName) {
    catchNullptr(  head  , TreeIsNull);
    catchNullptr(fileName, TreeIsNull);

    char *buffer = nullptr;
    int err = assignBuffer(&buffer, fileName);
    if (err) return err;

    err = treeBaseScanf(&(head -> tree), &buffer);
    if (err) return err;

    // fprintf(stderr, "Suka!\n");

    return TreeIsOk;
}

static int assignBuffer(char **buffer, const char *fileName) {
    catchNullptr(fileName, EXIT_FAILURE);

    size_t fileSize = getFileSize(fileName);

    FILE *stream = fopen(fileName, "r");
    catchNullptr(stream, EXIT_FAILURE);

    *buffer = (char *) calloc(fileSize + 1, sizeof(char));
    size_t gotSymbols = fread(*buffer, sizeof(char), fileSize, stream);
    CUR_STR[gotSymbols] = '\0';

    if (!feof(stream)) return EXIT_FAILURE;

    fclose(stream);

    return EXIT_SUCCESS;
}

static char *flashBuf(char *buffer);

static char* skipBuf(char *buffer);

static size_t getOperand(char c);

static char *getVar(char *buf);

static double getNum(char *buf);

static size_t getType(char *buf);

int treeBaseScanf(TreeNode **node, char **buffer) {
    catchNullptr(buffer, EXIT_FAILURE);

    int err = EXIT_SUCCESS;
    
    CUR_STR = flashBuf(CUR_STR);

    // fprintf(stderr, "!%c!\n", *buffer);
    // fprintf(stderr, "Blyat %c\n", **buffer);

    if (CUR_SYMB != '(') return EXIT_FAILURE;

    NEXT_SYMB;
    CUR_STR = flashBuf(CUR_STR);

    if (CUR_SYMB == ')') return EXIT_FAILURE;

    if (CUR_SYMB != '(') {
        enum NodeType type = (NodeType) getType(CUR_STR);
        // fprintf(stderr, "%zu == %zu\n", type, getType(CUR_STR));
        if (type == Numeral) {
            double data = getNum(CUR_STR);
            // fprintf(stderr, "%lf, buf: %s", data, CUR_STR);
            CUR_STR     = skipBuf(CUR_STR);
            NEXT_SYMB;

            return newNumNode(node, type, data);
        } else {
            char  *data = getVar(CUR_STR);
            CUR_STR     = skipBuf(CUR_STR);
            NEXT_SYMB;

            return newVarNode(node, type, data);
        }
    }

    enum NodeType curOp = Operand;

    err = newOpNode(node, curOp, 0);
    
    err |= treeBaseScanf(&((*node) -> lft), buffer);

    CUR_STR = flashBuf(CUR_STR);
    (*node) -> data.op = getOperand(CUR_SYMB);
    CUR_STR = skipBuf(CUR_STR);

    err |= treeBaseScanf(&((*node) -> rgt), buffer);

    if (CUR_SYMB != ')') return EXIT_FAILURE;
    NEXT_SYMB;

    return err;
}

#include "source/DSL.h"

TreeNode* treeDifferential(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);

    TreeNode *result = nullptr;

    switch (node -> type) {
        case Numeral:
            *err |= newNumNode(&result, Numeral, 0);
            if (*err) return nullptr;

            return result;
        
        case Varriable: 
            *err |= newNumNode(&result, Numeral, 1);
            if (*err) return nullptr;

            return result;

        case Operand:
            switch(node -> data.op) {
                case Add: return ADD(dL, dR);
                case Sub: return SUB(dL, dR);
                case Mul: return ADD(MUL(dL, cR), MUL(cL, dR));
                case Div: return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));

                default: return nullptr;
            }
        
        default: return nullptr;
    }
}

TreeNode* treeCopy(TreeNode *node, int *err) {
    catchNullptr(node, nullptr);


    if (node -> type == Operand) {
        switch(node -> data.op) {
            case Add: return ADD(cL, cR);
            case Sub: return SUB(cL, cR);
            case Mul: return MUL(cL, cR);
            case Div: return DIV(cL, cR);

            default: return nullptr;
        }
    }

    TreeNode *result = nullptr;

    if (node -> type == Numeral)
        *err |= newNumNode(&result, node -> type, node -> data.num);
    else if (node -> type == Varriable)
        *err |= newVarNode(&result, node -> type, node -> data.var);
    else
        *err = TreeTypeIsNone;

    if (*err) return nullptr;

    return result;
}

TreeNode* treeAdd(TreeNode *node1, TreeNode *node2, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, Add);
    if (*err) return nullptr;

    result -> lft = node1;
    result -> rgt = node2;

    return result;
}

TreeNode *treeSub(TreeNode *node1, TreeNode *node2, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, Sub);
    if (*err) return nullptr;

    result -> lft = node1;
    result -> rgt = node2;

    return result;
}

TreeNode *treeMul(TreeNode *node1, TreeNode *node2, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, Mul);
    if (*err) return nullptr;

    result -> lft = node1;
    result -> rgt = node2;

    return result;
}

TreeNode *treeDiv(TreeNode *node1, TreeNode *node2, int *err) {
    catchNullptr(node1, nullptr);
    catchNullptr(node2, nullptr);

    TreeNode *result = nullptr;
    *err = newOpNode(&result, Operand, Div);
    if (*err) return nullptr;

    result -> lft = node1;
    result -> rgt = node2;

    return result;
}


int treePrintEquat(Tree *head, const char *fileName) {
    catchNullptr(  head  , TreeIsNull);
    catchNullptr(fileName, TreeIsNull);

    FILE *stream = fopen(fileName, "w");
    catchNullptr(stream, TreeFileInErr);

    int err = treePrintNode(head -> tree, stream);
    if (err) return err;

    fclose(stream);

    return TreeIsOk;
}

int treePrintNode(TreeNode *node, FILE *stream) {
    catchNullptr( node , TreeIsNull);
    catchNullptr(stream, TreeIsNull);

    int      err    = EXIT_SUCCESS;

    switch (node -> type) {
        case Numeral:
            fprintf(stream, "%d", node -> data.num);
            return TreeIsOk;
        
        case Varriable: 
            fprintf(stream, "%s", node -> data.var);
            return TreeIsOk;

        case Operand:
            fprintf(stream, "(");
            err |= treePrintNode(node -> lft, stream);
            switch(node -> data.op) {
                case Add: 
                    fprintf(stream, " + ");
                    break;
                case Sub: 
                    fprintf(stream, " - ");
                    break;
                case Mul: 
                    fprintf(stream, " * ");
                    break;
                case Div: 
                    fprintf(stream, " / ");
                    break;
            }
            err |= treePrintNode(node -> rgt, stream);
            fprintf(stream, ")");

            return err;
        
        default: return TreeFileOutErr;
    }
} 

const char *treeGraphVizDump(Tree *tree, const char *fileName, int cmd) {
    if (fileName == nullptr) return nullptr;
    if (  tree   == nullptr) return nullptr;

    FILE *stream = fopen(fileName, "w");
    if (stream == nullptr) return nullptr;

    fprintf(stream, "digraph Tree {\n"
                    "   rankdir = TB;\n");

    fprintf(stream, "   subgraph varriables {\n"
                    "       node[shape = component, style = filled, fillcolor = orange];\n"

                    "         Tree   [label = \"Head = %08X\"];\n"
                    "        Status  [label = \"Status = %d\"];\n"
                    "   }\n",
            tree -> tree, tree -> status);

    fprintf(stream, "   subgraph Nodes {\n"
                    "       node[shape = Mrecord, style = filled, fillcolor = lightgreen];\n");
    size_t cnt = 1;
    treePrintNodeGrVz(tree -> tree, &cnt, stream);
    fprintf(stream, "   }\n");
    fprintf(stream, "   Tree->node1;\n}\n");

    fclose(stream);

    static char picName[MAX_CMD_SIZE] = "";
    sprintf(picName, "logs/logPic%zu.png", cmd);

    char CmdBuffer[MAX_CMD_SIZE] = {0};
    sprintf(CmdBuffer, "dot -Tpng %s -o %s", fileName, picName);

    if (system(CmdBuffer)) return nullptr;

    FILE *LogGraph = fopen("logs/HtmlLog.html", "w");
    
    fprintf(LogGraph, "<center>\n<h1>\nPicture[%zu]\n</h1>\n</center>\n", cmd);
    fprintf(LogGraph, "<img src= logPic%zu.png />\n", cmd);

    fclose(LogGraph);

    return picName;
}


void treePrintNodeGrVz(TreeNode *tree, size_t *cur, FILE *stream) {
    if (tree == nullptr || cur == nullptr || stream == nullptr) return;

    int nodeNum = *cur;

    fprintf(stream, "       node%zu[label = \"{ Node: %zu |", nodeNum, nodeNum);
    // fprintf(stderr, "%d\n", tree -> type);
    switch(tree -> type) {
        case Numeral:
            fprintf(stream, "Type: Numeral | Data: %lg } |", tree -> data.num);
            break;

        case Varriable:
            fprintf(stream, "Type: Varriable | VarName: %s } |", tree -> data.var);
            break;

        case Operand:
            fprintf(stream, "Type: Operand | Operand:");
            switch(tree -> data.op) {
                case Add: 
                    fprintf(stream, " + }|");
                    break;
                case Sub: 
                    fprintf(stream, " - }|");
                    break;
                case Mul: 
                    fprintf(stream, " * }|");
                    break;
                case Div: 
                    fprintf(stream, " / }|");
                    break;
            }
            break;
    }
    fprintf(stream, "LeftSon: %08X | RigthSon: %08X\"];\n",
                     tree -> lft,    tree -> rgt
            );
    
    if (tree -> lft != nullptr) {
        (*cur)++;
        fprintf(stream, "       node%zu -> node%zu[xlabel = \"lft\", color = \"darkblue\"];\n", nodeNum, *cur);
        treePrintNodeGrVz(tree -> lft, cur, stream);
    }
    if (tree -> rgt != nullptr) {
        (*cur)++;
        fprintf(stream, "       node%zu -> node%zu[xlabel = \"rgt\", color = \"darkgreen\"];\n", nodeNum, *cur);
        treePrintNodeGrVz(tree -> rgt, cur, stream);
    }
}

static void myfPrintf(FILE *stream = nullptr, const char *format = nullptr, ...) {
    if (format == nullptr)
        return;

    va_list arguments;
    va_start (arguments, format);
    vfprintf(stderr, format, arguments);
    if (stream != nullptr)
        vfprintf(stream, format, arguments);
    va_end(arguments);
}


void treePrintErrorMessage(int error) {
    if (error == 0) {
        myfPrintf(LogFile, "No Errors occured :)\n");
        return;
    }

    size_t numberOfErrors = 0;
    for (size_t currentError = 0; currentError < 20; currentError++)
        if (error & (1 << currentError))
            numberOfErrors++;
    myfPrintf(LogFile, "Program stopped with %d errors: \n", numberOfErrors);

    size_t currentError = 1;
    if (error & (1 <<  0))
        myfPrintf(LogFile, "%zu)  Struct Tree was nullptr!\n", currentError++);
    if (error & (1 <<  1))
        myfPrintf(LogFile, "%zu)  Tree is already full!\n", currentError++);
    if (error & (1 <<  2))
        myfPrintf(LogFile, "%zu)  Tree is already empty!\n", currentError++);
    if (error & (1 <<  3))
        myfPrintf(LogFile, "%zu)  tree -> head cannot be erased!\n", currentError++);
    if (error & (1 <<  4))
        myfPrintf(LogFile, "%zu)  Data in struct Tree was Null!\n", currentError++);
    if (error & (1 <<  5))
        myfPrintf(LogFile, "%zu)  Next in struct Tree was Null!\n", currentError++);
    if (error & (1 <<  6))
        myfPrintf(LogFile, "%zu)  There is no element with this index!\n", currentError++);
    if (error & (1 <<  7))
        myfPrintf(LogFile, "%zu)  Prev in struct Tree was Null!\n", currentError++);
    if (error & (1 <<  8))
        myfPrintf(LogFile, "%zu)  Struct Tree was inactive!\n", currentError++);
    if (error & (1 <<  9))
        myfPrintf(LogFile, "%zu)  Data in struct Tree was POISONED!\n", currentError++);
    if (error & (1 << 10))
        myfPrintf(LogFile, "%zu)  The System of free blocks is damaged in struct Tree\n", currentError++);
    if (error & (1 << 11))
        myfPrintf(LogFile, "%zu)  Struct Tree was Destructed two times!\n", currentError++);
    if (error & (1 << 12))
        myfPrintf(LogFile, "%zu)  Struct Tree was Constructed two times!\n", currentError++);
    if (error & (1 << 13))
        myfPrintf(LogFile, "%zu)  GraphViz execution error!\n", currentError++);
}

void treeLogClose() {
    if (LogFile != nullptr)
        fclose(LogFile);
}

// void vizLogClose() {
//     fclose(LogGraph);
// }

void flash() {
    int c = getchar();
    while (c != '\n' && c != EOF) c = getchar();
}

static char* skipBuf(char *buffer){
    while (*buffer != '(' && *buffer != ')') ++buffer;

    return buffer;
}

static char *flashBuf(char *buffer) {
    while (*buffer == '\n' || *buffer == ' ') ++buffer;

    return buffer;
}

static size_t getOperand(char c) {
    switch (c) {
        case '+':  return Add;
        case '-':  return Sub;
        case '*':  return Mul;
        case '\\': return Div;

        default:   return None;
    }
}

static size_t getType(char *buf) {
    if (isalpha(*buf))
        return Varriable;
    return Numeral;
}

static char *getVar(char *buf) {
    char *varName = (char *) calloc(MAX_STRING_SIZE, sizeof(char));
    size_t gotSymb = sscanf(buf, "%s)", varName);
    if (!gotSymb) return nullptr;
    
    varName[gotSymb] = '\0';

    return varName;
}

static double getNum(char *buf) {
    double num = 0;
    sscanf(buf, "%lf", &num);

    return num;
}

static size_t getFileSize(const char *fileName) {
    catchNullptr(fileName, EXIT_FAILURE);

    struct stat buf = {};
    if (stat(fileName, &buf)) {
        fprintf(stderr, "An Error in reading file occured\n");
        return 0;
    }

    return buf.st_size;
}