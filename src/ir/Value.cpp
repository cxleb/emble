#include "ir/value.h"
#include <iostream>
namespace ir {
	void Value::print() {
		std::cout << "Undefined Value!";
	}

	Number* makeNumber32(int Value) {
		Number* num = new Number();
		num->Value = Value;
		num->type = Types::Int32;
		return num;
	}

	VariablePointer* makePointer(int to) {
		VariablePointer* pointer = new VariablePointer();
		pointer->pointing = to;
		pointer->type = Types::VPointer;
		return pointer;
	}

	FunctionPointer* makeFunctionPointer(std::string to){
		FunctionPointer* p = new FunctionPointer();
		p->pointing = to;
		p->type = Types::FPointer;
		return p;
	}

	BlockPointer* makeBlockPointer(int bnum){
		BlockPointer* p = new BlockPointer();
		p->pointing = bnum;
		p->type = Types::BPointer;
		return p;
	}
}