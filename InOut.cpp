#include "source/tree.h"
#include "source/config.h"
#include "headers/calc.h"
#include "source/DSL.h"
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define CUR_SYMB (*CUR_STR)

#define NEXT_SYMB CUR_STR++

#define CUR_STR (*buffer)

static assignBuffer(char **buffer, const char *fileName);

static size_t getFileSize(const char *fileName);

int treeLoadBase(Tree *head, const char *fileName, Vocabulary *varList) {
    catchNullptr(  head  , TreeIsNull);
    catchNullptr(fileName, TreeIsNull);

    char *buffer = nullptr;
    size_t err = assignBuffer(&buffer, fileName);
    if (err) return err;

    head -> tree = getG(&buffer, varList, &err);
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

static int getOneArg(TreeNode **node, enum OperandType type, char **buffer, Vocabulary *varList);

static int getTwoArgs(TreeNode **node, char **buffer, Vocabulary *varList);

static int getArg(TreeNode **node, char **buffer, Vocabulary *varList);

int treeBaseScanf(TreeNode **node, char **buffer, Vocabulary *varList) {
    catchNullptr(buffer, TreeFileInErr);

    int err = EXIT_SUCCESS;
    
    CUR_STR = flashBuf(CUR_STR);

    // fprintf(stderr, "!%c!\n", *buffer);
    // fprintf(stderr, "Blyat %c\n", **buffer);

    if (CUR_SYMB != '(') return TreeFileInErr;

    NEXT_SYMB;
    CUR_STR = flashBuf(CUR_STR);

    if (CUR_SYMB == ')') return TreeFileInErr;

    if (!strncmp(CUR_STR, "sin", 3)) {
        err = getOneArg(node, Sin, buffer, varList);
        return err;
    }

    if (!strncmp(CUR_STR, "cos", 3)) {
        err = getOneArg(node, Cos, buffer, varList);
        return err;
    }

    if (!strncmp(CUR_STR, "ln", 2)) {
        err = getOneArg(node, Log, buffer, varList);
        return err;
    }

    if (CUR_SYMB != '(') {
        err = getArg(node, buffer, varList);
        return err;
    }

    err = getTwoArgs(node, buffer, varList);

    return err;
}

int treePrintEquat(Tree *head, Vocabulary *varList, const char *fileName) {
    catchNullptr(  head  , TreeIsNull);
    catchNullptr(fileName, TreeIsNull);

    FILE *stream = fopen(fileName, "w");
    catchNullptr(stream, TreeFileInErr);

    fprintf(stream, "$$");
    int err = treePrintNode(head -> tree, varList, stream);
    if (err) return err;
    fprintf(stream, "$$\n");

    fclose(stream);

    return TreeIsOk;
}

int treePrintNode(TreeNode *node, Vocabulary *varList, FILE *stream, bool cut) {
    catchNullptr( node , TreeIsNull);
    catchNullptr(stream, TreeIsNull);

    int err = EXIT_SUCCESS;

    if (cut && node -> nodeName != nullptr) {
        fprintf(stream, node -> nodeName);
        return TreeIsOk;
    }

    switch (node -> type) {
        case Numeral:
            fprintf(stream, "%lg", node -> data.num);
            return TreeIsOk;
        
        case Varriable: 
            fprintf(stream, "%s", node -> data.var);
            return TreeIsOk;

        case Operand:
            switch(node -> data.op) {
                case Add:
                    fprintf(stream, "(");
                    err |= treePrintNode(node -> lft, varList, stream, 1);
                    fprintf(stream, " + ");
                    err |= treePrintNode(node -> rgt, varList, stream, 1);
                    fprintf(stream, ")");


                    break;

                case Sub:
                    fprintf(stream, "(");
                    err |= treePrintNode(node -> lft, varList, stream, cut);
                    fprintf(stream, " - ");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, ")");

                    break;

                case Mul:
                    err |= treePrintNode(node -> lft, varList, stream, cut);
                    fprintf(stream, " \\cdot ");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);

                    break;

                case Div: 
                    fprintf(stream, " \\frac{ ");
                    err |= treePrintNode(node -> lft, varList, stream, cut);
                    fprintf(stream, "} {");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, "}");

                    break;

                case Pow:
                    if (nOp(L(node)) >= FuncStart)
                        fprintf(stream, "(");
                    err |= treePrintNode(node -> lft, varList, stream, cut);
                    if (nOp(L(node)) >= FuncStart)
                        fprintf(stream, ")");
                    fprintf(stream, "^{");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, "}");

                    break;

                case Sin:
                    fprintf(stream, " \\sin{");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, "}");

                    break;

                case Cos:
                    fprintf(stream, " \\cos{");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, "}");

                    break;

                case Log:
                    fprintf(stream, " \\ln{");
                    err |= treePrintNode(node -> rgt, varList, stream, cut);
                    fprintf(stream, "}");

                    break;
            }

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

    fprintf(stream, "       node%zu[label = \"{ Node: %zu | { ", nodeNum, nodeNum);
    // fprintf(stderr, "%d\n", tree -> type);
    switch(tree -> type) {
        case Numeral:
            fprintf(stream, "Type: Numeral | Data: %lg", tree -> data.num);
            break;

        case Varriable:
            fprintf(stream, "Type: Varriable | VarName: %s", tree -> data.var);
            break;

        case Operand:
            fprintf(stream, "Type: Operand | Operand:");
            switch(tree -> data.op) {
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
                case Pow: 
                    fprintf(stream, " ^ ");
                    break;
                case Sin:
                    fprintf(stream, " sin");
                    break;
                case Cos:
                    fprintf(stream, " cos");
                    break;
                case Log:
                    fprintf(stream, " ln");
                    break;
            }
            break;
    }
    fprintf(stream, " } |  Size: %zu  | LeftSon: %08X | RigthSon: %08X}\"];\n",
                         tree -> size,  tree -> lft,    tree -> rgt
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

static int getArg(TreeNode **node, char **buffer, Vocabulary *varList) {
    catchNullptr( node ,  TreeIsNull  );
    catchNullptr(buffer, TreeFileInErr);

    enum NodeType type = (NodeType) getType(CUR_STR);
    // fprintf(stderr, "%zu == %zu\n", type, getType(CUR_STR));
    if (type == Numeral) {
        double data = getNum(CUR_STR);
        // fprintf(stderr, "%lf, buf: %s", data, CUR_STR);
        CUR_STR     = skipBuf(CUR_STR);
        NEXT_SYMB;

        return newNumNode(node, type, data);
    } 
    char  *data = getVar(CUR_STR);
    CUR_STR     = skipBuf(CUR_STR);
    NEXT_SYMB;
    
    varList -> var[varList -> size++] = data;

    return newVarNode(node, type, data);
}

static int getOneArg(TreeNode **node, enum OperandType type, char **buffer, Vocabulary *varList) {
    catchNullptr( node ,  TreeIsNull  );
    catchNullptr(buffer, TreeFileInErr);

    int err = newOpNode(node, Operand, type);
    CUR_STR = skipBuf(CUR_STR); 
    err |= treeBaseScanf(&((*node) -> rgt), buffer, varList);
    
    if (CUR_SYMB != ')') return TreeFileInErr;
    NEXT_SYMB;

    return err;
}

static int getTwoArgs(TreeNode **node, char **buffer, Vocabulary *varList) {
    catchNullptr( node ,  TreeIsNull  );
    catchNullptr(buffer, TreeFileInErr);

    int err = newOpNode(node, Operand, 0);
    
    err |= treeBaseScanf(&((*node) -> lft), buffer, varList);

    CUR_STR = flashBuf(CUR_STR);
    (*node) -> data.op = getOperand(CUR_SYMB);
    CUR_STR = skipBuf(CUR_STR);

    err |= treeBaseScanf(&((*node) -> rgt), buffer, varList);

    if (CUR_SYMB != ')') return TreeFileInErr;
    NEXT_SYMB;

    return err;
}

static size_t getOperand(char c) {
    switch (c) {
        case '+':  return Add;
        case '-':  return Sub;
        case '*':  return Mul;
        case '/': return Div;
        case '^':  return Pow;

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

void flash() {
    int c = getchar();
    while (c != '\n' && c != EOF) c = getchar();
}

static char* skipBuf(char *buffer){
    while (*buffer != '(' && *buffer != ')' && *buffer != '\0') ++buffer;

    return buffer;
}

static char *flashBuf(char *buffer) {
    while ((*buffer == '\n' || *buffer == ' ') && *buffer != '\0') ++buffer;

    return buffer;
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