#define  dL   treeDifferential(node -> lft, varList, err, stream)
#define  dR   treeDifferential(node -> rgt, varList, err, stream)
#define  cL       treeCopy    (node -> lft, varList, err)
#define  cR       treeCopy    (node -> rgt, varList, err)

#define  ADD(NODE1, NODE2) treeDoubleArgumentOper(NODE1, NODE2, Add, err)
#define  SUB(NODE1, NODE2) treeDoubleArgumentOper(NODE1, NODE2, Sub, err)
#define  MUL(NODE1, NODE2) treeDoubleArgumentOper(NODE1, NODE2, Mul, err)
#define  DIV(NODE1, NODE2) treeDoubleArgumentOper(NODE1, NODE2, Div, err)
#define  POW(NODE1, NODE2) treeDoubleArgumentOper(NODE1, NODE2, Pow, err)
#define  COS(    NODE    )  treeOneArgumentOper  (     NODE   , Cos, err)
#define  SIN(    NODE    )  treeOneArgumentOper  (     NODE   , Sin, err)
#define  LOG(    NODE    )  treeOneArgumentOper  (     NODE   , Log, err)
#define  REV(    NODE    )        treeRev        (     NODE   ,      err)
#define  NEG(    NODE    )        treeNeg        (     NODE   ,      err)

#define  d_aX  MUL(MUL(treeCopy(node, varList, err), LOG(cL)), dR)
#define  d_xA  MUL(MUL(cR, POW(cL, SUB(cR, one))), dL)

#define  nType(NODE) (NODE) -> type
#define   nNum(NODE) (NODE) -> data.num
#define   nVar(NODE) (NODE) -> data.var
#define    nOp(NODE) (NODE) -> data.op
#define      L(NODE) (NODE) -> lft
#define      R(NODE) (NODE) -> rgt

#define DESES (varList -> designations)