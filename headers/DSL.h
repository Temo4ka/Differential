#define CUR_STR  (*buffer)
#define CUR_SYM (**buffer)
#define NEXT_SYM ++(*buffer)

#define catchNullptr2(POINTER, RETURN_VALUE,...) {                                                                  \
    if ((POINTER) == nullptr) {                                                                                     \
        fprintf(stderr, "%s pointer at %s at %s(%d) is NULL\n", #POINTER, __PRETTY_FUNCTION__, __FILE__, __LINE__);  \
        __VA_ARGS__                                                                                                   \
        return RETURN_VALUE;                                                                                           \
    }                                                                                                                   \
}

#define ERR_EXE(ERROR_CODE) {                                                     \
    *err |= ERROR_CODE;                                                            \
    fprintf(stderr, "%s at %s(%d);\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);    \
    return nullptr;                                                                  \
}
