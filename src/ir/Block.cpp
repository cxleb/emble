#include "ir/block.h"

namespace ir {
    void block::addInstruction(instruction* ins) {
        this->instructions.push_back(ins);
    }
    
    void block::print() {
        for (instruction* ins : this->instructions) {
            printf("\t");
            ins->print();
        }
    }
}