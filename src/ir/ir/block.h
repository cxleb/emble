#pragma once
#include "ir/instruction.h"
#include <deque>

namespace ir {
	struct block{
		virtual ~block() {}
		std::deque<instruction*> instructions;
		virtual void addInstruction(instruction*);
		virtual void print();
	};
}