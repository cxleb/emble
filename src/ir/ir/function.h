#pragma once
#include "block.h"
#include <map>
#include <string>

namespace ir {
    struct function {
        virtual ~function() { }
        int currentBlock;
        int currentPoint = 0;
        std::string Name;
        std::map<int, block*> blocks;
        std::map<int, int> alloc;
        virtual void addBlock(int blockNum, block* block);
        virtual int addVariable(int size);
        virtual void print();
    };
}