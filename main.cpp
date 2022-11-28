#include "source/task.h"
#include "source/config.h"
#include <string.h>

int main(int argc, char *argv[]) {
    const char  *inputFileName =  DEFAULT_IN_FILE_NAME;
    const char *outputFileName = DEFAULT_OUT_FILE_NAME;

    int err = 0;

    switch(argc) {
        case 2:
            inputFileName = argv[1];
            break;

        case 3:
             inputFileName = argv[1];
            outputFileName = argv[2];
            break;
        
        default: break;
    }
    size_t pic = 0;

    Tree head = {};
    err = treeCtor(&head);
    if (err) return EXIT_FAILURE;

    Vocabulary varList = {};

    FILE *stream = texStart(outputFileName);
    if (err) return EXIT_FAILURE;

    err = treeLoadBase(&head, inputFileName, &varList);
    if (err) return EXIT_FAILURE;

    treeGraphVizDump(&head, "logs/graph.dot", ++pic);

    err = texPrintInit(&head, &varList, stream);
    if (err) return EXIT_FAILURE;

    Tree difTree = {};
    err = treeCtor(&difTree);
    if (err) return EXIT_FAILURE;

    difTree.tree = treeDifferential(head.tree, &varList, &err, stream);
    if (err) return EXIT_FAILURE;

    treeGraphVizDump(&difTree, "logs/graph.dot", ++pic);

    err = texSimplify(&difTree, &varList, stream);
    if (err) return EXIT_FAILURE;

    treeGraphVizDump(&difTree, "logs/graph.dot", ++pic);

    err = texTeylor(&head, &varList, ((argc > 3)? atoi(argv[3]) : 5), stream);
    if (err) return err;

    treeGraphVizDump(&difTree, "logs/graph.dot", ++pic);

    err = texEnd(stream);
    if (err) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
