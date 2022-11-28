#include "source/task.h"
#include "source/DSL.h"

FILE* texStart(const char *fileName) {
    catchNullptr(fileName, nullptr);

    FILE *stream = fopen(fileName, "w");
    catchNullptr(stream, nullptr);

    fprintf(stream,
    "\\documentclass[12pt,a4paper,fleqn]{article} \n"
    "\\usepackage[utf8]{inputenc} \n"
    "\\usepackage{amssymb, amsmath, multicol} \n"
    "\\usepackage[russian]{babel} \n"
    "\\usepackage{graphicx} \n"
    "\\usepackage[shortcuts,cyremdash]{extdash} \n"
    "\\usepackage{wrapfig} \n"
    "\\usepackage{floatflt} \n"
    "\\usepackage{lipsum} \n"
    "\\usepackage{euler} \n" 

    "\\oddsidemargin= 6mm \n"
    "\\textwidth=150mm \n"
    "\\headheight=-32.4mm \n"
    "\\textheight=277mm \n"
    "\\parindent=0pt \n"
    "\\parskip=8pt \n"
    "\\pagestyle{empty} \n"

    "\\author{А.Д. Букин} \n"
    "\\title{Матиматический Анализ\\\\ Издание деcятнадцатое, исправленное}"
    "\\date{Пусть b = ояб, тогда Нbрь 2022} \n"

    "\\begin{document} \n"

    "\\maketitle \n"

    "Данная книга целиком и полностью внедрит и посвятит вас в основу математического анализа, поможет разобраться с основными понятиями и просвятит все ваши адренорецепторы в гемиспазм и гиперестезию аксонометрии. Гониометрия и лемниската непрерывных на додекаидре функции довольно кратко и дакреоденитно изложены так, что любому ириту будет очевиден тот факт, что трохоида уникурсального электропунктура расположен биопараллельно циссоиде эвольвенты. \n"
    "\\\\ \\begin{center} \n"
        "Начнём с простого взятия производной \n"
    "\\end{center} \n"
    );

    return stream;
}

int texPrintInit(Tree *head, Vocabulary *varList, FILE *stream) {
    catchNullptr( head ,   TreeIsNull  );
    catchNullptr(stream, TreeFileOutErr);
    
    fprintf(stream, "Пусть $$f(x) = ");

    int err = treePrintNode(head -> tree, varList, stream);
    if (err) return err;

    fprintf(stream, "$$\n");

    return TreeIsOk;
}

int texSimplify(Tree *head, Vocabulary *varList, FILE *stream) {
    catchNullptr( head ,   TreeIsNull  );
    catchNullptr(stream, TreeFileOutErr);

    fprintf(stream, "После упрощения $$f^\\prime (x) = ");

    int err = treeSimplify(&(head -> tree));
    if (err) return err;

    err = treePrintNode(head -> tree, varList, stream);
    if (err) return err;
    
    fprintf(stream, "$$\n");
    
    return TreeIsOk;
}

int texTeylor(Tree *head, Vocabulary *varList, size_t n, FILE *stream) {
    catchNullptr(  head ,   TreeIsNull  );
    catchNullptr(varList,   TreeIsNull  );
    catchNullptr( stream, TreeFileOutErr);

    fprintf(stream, "Ряд Тейлора до $(x^%zu)$ $$f^\\prime (x) = ", n);

    // fprintf(stderr, "%d", n);
    int err = treePrintNTeylor(head, varList, n, stream);
    if (err) return err;
    
    fprintf(stream, "$$\n");
    
    return TreeIsOk;
}


int texEnd(FILE *stream) {
    catchNullptr(stream, TreeFileOutErr);

    fprintf(stream, "\\\\ \\end{document}\n");

    fclose(stream);

    return TreeFileOutErr;
}