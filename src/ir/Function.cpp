#include "ir/function.h"
#include <iostream>
namespace ir {
	void function::addBlock(int blockNum, block* block) {
		this->blocks[blockNum] = block;
	}

	int function::addVariable(int size) {
		this->alloc[this->currentPoint] = size;
		int temp = this->currentPoint;
		this->currentPoint += size;
		return temp;
	}

	void function::print() {
		std::cout << "Function: " << this->Name << "\n";
		for (std::map<int, block*>::iterator it = this->blocks.begin(); it != this->blocks.end(); ++it) {
			std::cout << "Block: " << it->first << '\n';
			it->second->print();
		}
	}
}