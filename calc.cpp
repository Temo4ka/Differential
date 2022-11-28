#include "source/tree.h"
#include "headers/DSL.h"
#include "headers/calc.h"
#include <math.h>
#include <ctype.h>
#include <string.h>

//----------------------------------------
//! G = E, '\0'
//! E = T {[+ -] T}*
//! T = D {[* /] D}*
//! D = U {[^] U}*
//! U = cos(E) | sin(E) | ln(E) | -E | P
//! P = (E) | N | V
//! N = ['0' - '9']+
//! V = ['a' - 'z' | 'A' - 'Z']+
//-----------------------------------------

static TreeNode* getCos(char **buffer, Vocabulary *varList, size_t *err);

static TreeNode* getSin(char **buffer, Vocabulary *varList, size_t *err);

static TreeNode* getLog(char **buffer, Vocabulary *varList, size_t *err);

static TreeNode* getNeg(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getG(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    TreeNode* node = getE(buffer, varList, err);

    fprintf(stderr, "%c\n", CUR_SYM);
    if (CUR_SYM != '\n' && CUR_SYM != '\0') ERR_EXE(calcEndOfProgramErr);
    
    return node;
}

TreeNode* getE(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    TreeNode* node1 = getT(buffer, varList, err);

    while (CUR_SYM == '+' || CUR_SYM == '-') {
        TreeNode* node  = nullptr;

        if (CUR_SYM == '+')
            *err |= newOpNode(&node, Operand, Add);
        else
            *err |= newOpNode(&node, Operand, Sub);

        NEXT_SYM;

        TreeNode* node2 = getT(buffer, varList, err);

        if (node1 == nullptr) ERR_EXE(calcGetE_Error);
        if (node2 == nullptr) ERR_EXE(calcGetE_Error);

        node -> lft = node1;
        node -> rgt = node2;   

        node1 = node;         
    }

    return node1;
}

TreeNode* getT(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    TreeNode* node1 = getD(buffer, varList, err);

     while (CUR_SYM == '*' || CUR_SYM == '/') {
        TreeNode* node  = nullptr;

        if (CUR_SYM == '*')
            *err |= newOpNode(&node, Operand, Mul);
        else
            *err |= newOpNode(&node, Operand, Div);

        NEXT_SYM;

        TreeNode* node2 = getT(buffer, varList, err);

        if (node1 == nullptr) ERR_EXE(calcGetE_Error);
        if (node2 == nullptr) ERR_EXE(calcGetE_Error);

        node -> lft = node1;
        node -> rgt = node2;

        node1 = node;            
    }

    return node1;
}

TreeNode* getD(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    TreeNode* node1 = getU(buffer, varList, err);
    catchNullptr2(buffer, nullptr, *err |= calcGetD_Error;);

    while (CUR_SYM == '^') {
        NEXT_SYM;

        TreeNode* node  =     nullptr      ;
        *err |= newOpNode(&node, Operand, Pow);

        TreeNode* node2 = getU(buffer, varList, err);
        catchNullptr2(buffer, nullptr, *err |= calcGetD_Error;);

        node -> lft = node1;
        node -> rgt = node2;

        node1 = node;
    }

    return node1;
}

TreeNode* getU(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    if (!strncmp("cos(", CUR_STR, 4)) {
        TreeNode* node = getCos(buffer, varList, err);
        if (node == nullptr || *err) ERR_EXE(calcGetU_Error);

        return node;
    }

    if (!strncmp("sin(", CUR_STR, 4)) {
        TreeNode* node = getSin(buffer, varList, err);
        if (node == nullptr || *err) ERR_EXE(calcGetU_Error);
        
        return node;
    }

    if (!strncmp("ln(", CUR_STR, 3)) {
        // fprintf(stderr, "Here\n");
        TreeNode* node = getLog(buffer, varList, err);
        if (node == nullptr || *err) ERR_EXE(calcGetU_Error);
        
        return node;
    }

    if (CUR_SYM == '-') {
        TreeNode* node = getNeg(buffer, varList, err);
        if (node == nullptr || *err) ERR_EXE(calcGetU_Error);

        return node;
    }

    TreeNode* node = getP(buffer, varList, err);

    if (node == nullptr || *err) ERR_EXE(calcGetU_Error);

    return node;
}

TreeNode* getP(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    if (CUR_SYM == '(') {
        NEXT_SYM;
        TreeNode* node = getE(buffer, varList, err);
        if (CUR_SYM != ')') ERR_EXE(calcGetP_Error);
        NEXT_SYM;
        
        return node;
    }

    TreeNode* node = getN(buffer, err);
    if (node == nullptr)
        node = getV(buffer, varList, err);
    catchNullptr2(node, nullptr, *err |= calcGetP_Error;);

    return node;
}

TreeNode* getN(char **buffer, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    Elem_t val = 0;

    if (CUR_SYM < '0' || CUR_SYM > '9') return nullptr;

    while (CUR_SYM >= '0' && CUR_SYM <= '9') {
        val = val * 10 + CUR_SYM - '0';
        NEXT_SYM;
    }

    TreeNode *node = nullptr;
    *err |= newNumNode(&node, Numeral, val);

    return node;
}

TreeNode* getV(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    char *newVar = (char *) calloc(MAX_VAR_SIZE, sizeof(char));
    char *curVar = newVar;

    // fprintf(stderr, "Here\n");
    if (!isalpha(CUR_SYM)) {
        *err |= calcGetV_Error;
        return nullptr;
    }

    while (isalpha(CUR_SYM)) {
        *curVar = CUR_SYM;
        NEXT_SYM;
        curVar++;
    }
    *curVar = '\0';

    varList -> var[varList -> size++] = newVar;

    TreeNode *node = nullptr;
    *err |= newVarNode(&node, Varriable, newVar);
    
    return node;
}

static TreeNode* getCos(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    CUR_STR += 4;

    TreeNode* arg  = getE(buffer, varList, err);
    TreeNode* node =      nullptr     ;

    *err |= newOpNode(&node, Operand, Cos);
    if (arg == nullptr || CUR_SYM != ')') ERR_EXE(calcGetCosError);

    NEXT_SYM;

    node -> rgt = arg;

    return node;
}

static TreeNode* getSin(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    CUR_STR += 4;

    TreeNode* arg  = getE(buffer, varList, err);
    TreeNode* node =      nullptr     ;

    *err |= newOpNode(&node, Operand, Sin);
    if (arg == nullptr || CUR_SYM != ')') ERR_EXE(calcGetSinError);

    NEXT_SYM;

    node -> rgt = arg;

    return node;
}

static TreeNode* getLog(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    CUR_STR += 3;

    TreeNode* arg  = getE(buffer, varList, err);
    TreeNode* node =      nullptr     ;

    *err |= newOpNode(&node, Operand, Log);
    if (arg == nullptr || CUR_SYM != ')') ERR_EXE(calcGetLogError);

    NEXT_SYM;

    node -> rgt = arg;

    return node;
}

static TreeNode* getNeg(char **buffer, Vocabulary *varList, size_t *err) {
    catchNullptr2(buffer, nullptr, *err |= calcNullCaught;);

    CUR_STR += 4;

    TreeNode* arg  = getE(buffer, varList, err);
    TreeNode* node =      nullptr     ;
    TreeNode* neg  =      nullptr     ;

    *err |= newOpNode (&node, Operand, Mul);
    *err |= newNumNode(&node, Numeral,  -1);

    if (arg == nullptr) ERR_EXE(calcGetLogError);

    NEXT_SYM;

    node -> lft = neg;
    node -> rgt = arg;

    return node;
}

//TODO: strncmp