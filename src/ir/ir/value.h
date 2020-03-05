#pragma once
#include "ir/types.h"
#include <string>

namespace ir {
    struct Value {
        virtual ~Value() {}
        Types type;
        virtual void print();
    };
    
    struct Number : Value {
        ~Number() {}
        uint64_t Value;
        virtual void print() override;
    };
    
    struct VariablePointer : Value {
        ~VariablePointer() {}
        int pointing;
        virtual void print() override;
    };
    
    Number* makeNumber32(int Value);
    VariablePointer* makePointer(int to);
    FunctionPointer* makeFunctionPointer(std::string to);
    BlockPointer* makeBlockPointer(int bnum);
}