#include <stdlib.h>
#include <stdio.h>
#include "config.h"

struct Buffer {
    const char *fileName = nullptr;

    FILE *stream = nullptr;
    
    char *buffer = nullptr;
    size_t size  =   0    ;

};

enum CalcErrors {
    calcOk              =       0,
    calcNullCaught      = 1 <<  0,
    calcGetE_Error      = 1 <<  1,
    calcGetT_Error      = 1 <<  2,
    calcGetD_Error      = 1 <<  3,
    calcGetP_Error      = 1 <<  4,
    calcGetU_Error      = 1 <<  5,
    calcGetN_Error      = 1 <<  6,
    calcGetV_Error      = 1 <<  7,
    calcBufferError     = 1 <<  8,
    calcGetCosError     = 1 <<  9,
    calcGetSinError     = 1 << 10,
    calcGetLogError     = 1 << 11,
    calcGetNegError     = 1 << 12,
    calcDevisionByZero  = 1 << 13,
    calcEndOfProgramErr = 1 << 14,
};

int bufferCtor(Buffer *buf, const char *fileName);

int bufferDtor(Buffer *buf);

TreeNode* getG(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getE(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getT(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getP(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getD(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getU(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getV(char **buffer, Vocabulary *varList, size_t *err);

TreeNode* getN(char **buffer, size_t *err);