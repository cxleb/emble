#pragma once
#include <deque>
#include "function.h"
namespace ir {
    struct module{
        virtual ~module() { }
        std::deque<function*> functions;
        virtual void addFunction(function* func);
        virtual void print();
    };
};p