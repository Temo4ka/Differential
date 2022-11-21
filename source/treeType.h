#pragma once

enum OperandType {
    Sin = 1,
    Cos = 2,
    Add = 3,
    Sub = 4,
    Mul = 5,
    Div = 6,
    Log = 7,
    Pow = 8,
};

enum  NodeType { 
      None    = 0,
    Varriable = 1,
     Operand  = 2,
     Numeral  = 3,
};

typedef char* Elem_t;

const  Elem_t       POISON     = nullptr;
const unsigned MAX_STRING_SIZE =  10000 ;