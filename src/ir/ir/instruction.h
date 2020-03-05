#pragma once
#include "ir/value.h"
#include <deque>

namespace ir {
    enum Opcodes {
        Set,
        Get,
        
        Add,
        Sub,
        Mul,
        Div,
        
        Call,
        Jmp,
        Cmp,
        Je,
        Jne
    };
    
    struct instruction : Value {
        virtual ~instruction() {}
        Opcodes opcode;
        virtual void print();
    };
    
    struct mathOpIns : Value {
        virtual ~mathOpIns() { }
        Value* lhs;
        Value* rhs;
    }	
    
    instruction* createAdd();
    instruction* createSet(VariablePointer* lhs);
    instruction* createGet(Value* value);
    instruction* createCall(FunctionPointer* pointer);
}