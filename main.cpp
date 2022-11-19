#include "source/tree.h"
#include "source/config.h"

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
    size_t pic = 1;

    Tree head = {};
    err = treeCtor(&head);
    if (err) return EXIT_FAILURE;

    err = treeLoadBase(&head, inputFileName);
    if (err) return EXIT_FAILURE;

    treeGraphVizDump(&head, "logs/graph.dot", pic);

    TreeNode *node = treeDifferential(head.tree, &err);
    if (err) return EXIT_FAILURE;

    err = treePrintEquat(&head, outputFileName);
    if (err) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}