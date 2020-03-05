#include "ir/instruction.h"

#include <iostream>

namespace ir {
	instruction* createSet(VariablePointer* lhs){
		instruction* ins = new instruction();
		ins->opcode = Opcodes::Set;
		ins->operand = lhs;
		return ins;
	}

	instruction* createGet(Value* value){
		instruction* ins = new instruction();
		ins->opcode = Opcodes::Get;
		ins->operand = value;
		return ins;
	}

	instruction* createAdd() {
		instruction* ins = new instruction();
		ins->opcode = Opcodes::Add;
		return ins;
	}

	instruction* createCall(FunctionPointer* pointer){
		instruction* ins = new instruction();
		ins->opcode = Opcodes::Call;
		ins->operand = pointer;
		return ins;
	}

	void instruction::print() {
		std::string ins;
		switch (this->opcode)
		{
		case Opcodes::Add:
			ins = "Add";
			break;
		case Opcodes::Sub:
			ins = "Sub";
			break;
		case Opcodes::Mul:
			ins = "Mul";
			break;
		case Opcodes::Div:
			ins = "Div";
			break;
		case Opcodes::Call:
			ins = "Call";
			break;
		case Opcodes::Jmp:
			ins = "Jmp";
			break;
		case Opcodes::Cmp:
			ins = "Cmp";
			break;
		case Opcodes::Je:
			ins = "Je";
			break;
		case Opcodes::Jne:
			ins = "Jne";
			break;
		default:
			break;
		}

		std::cout << ins;
		//for (Value* value : this->Values) {
		//	printf(" ");
		//	value->print();
		//	printf(",");
		//}
		printf("\n");
	}
}