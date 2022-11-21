#include "source/tree.h"
#include "source/config.h"
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

static FILE *LogFile  = fopen("logs/logFile.txt", "w");

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