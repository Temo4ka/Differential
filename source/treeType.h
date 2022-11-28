#pragma once

enum OperandType {
        Add   = 1,
        Sub   = 2,
        Mul   = 3,
        Div   = 4,
        Pow   = 5,
    FuncStart = 6,
        Sin   = 6,
        Cos   = 7,
        Log   = 8,
};

enum  NodeType { 
      None    = 0,
    Varriable = 1,
     Operand  = 2,
     Numeral  = 3,
};

typedef double Elem_t;

const  Elem_t       POISON     =   1e9  ;
const unsigned MAX_STRING_SIZE =  10000 ;