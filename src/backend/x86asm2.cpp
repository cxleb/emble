#include "x86asm.h"
#include <sstream>

void push(std::stringstream* strm, std::string cmd) {
	(*strm) << cmd << "\n";
}

std::string translateModule(ir::module* module) {
	std::stringstream stream;

	for (ir::function* func : module->functions) {
		stream << translateFunction(func);
	}


	return stream.str();
}

std::string translateFunction(ir::function* func) {
	std::stringstream stream;
	// put name of function at top
	stream << "_" <<func->Name << ":\n";

	// calculate size of the frame;
	int sizeOfFrame = 0;
	for (std::map<int, int>::iterator it = func->alloc.begin(); it != func->alloc.end(); ++it) {
		sizeOfFrame += it->second;
	}

	// setup stack frame asm
	stream << "push ebp\n";
	stream << "mov ebp, esp\n";
	stream << "sub ebp, " << std::to_string(sizeOfFrame) << "\n";

	// translate each block
	for (std::map<int, ir::block*>::iterator it = func->blocks.begin(); it != func->blocks.end(); ++it) {
		stream << translateBlock(func->Name, it->first, it->second);
	}

	stream << "mov esp, ebp\n";
	stream << "pop ebp\n";
	stream << "ret\n";

	return stream.str();
}

std::string translateBlock(std::string name, int bnum, ir::block* block) {
	std::stringstream stream;
	stream << "_" << name << "_b"<< std::to_string(bnum) << ":\n";

	for (ir::instruction* ins : block->instructions) {
		//for (ir::Value* val : ins->Values) {
		//	push(&stream, getValue(val));
		//}

		switch (ins->opcode)
		{
		case ir::Opcodes::Set:
			push(&stream, "pop eax");
			push(&stream, "mov DWORD [ebp - " + std::to_string(((ir::VariablePointer*) ins->operand)->pointing + 4) +" ], eax");
			break;
		case ir::Opcodes::Get:
			stream << getValue(ins->operand);
			break;
		case ir::Opcodes::Add:
			push(&stream, "pop edx");
			push(&stream, "pop eax");
			push(&stream, "add eax, edx");
			push(&stream, "push eax");
			break;
		case ir::Opcodes::Sub:
			push(&stream, "pop edx");
			push(&stream, "pop eax");
			push(&stream, "sub eax, edx"); 
			push(&stream, "push eax");
			break;
		case ir::Opcodes::Mul:
			stream << "mul" << "\n";
			break;
		case ir::Opcodes::Div:
			stream << "div" << "\n";
			break;
		case ir::Opcodes::Call:
			stream << "call" << " _" << ((ir::FunctionPointer*)ins->operand)->pointing << "\n";
			break;
		case ir::Opcodes::Jmp:
			stream << "jmp" << "\n";
			break;
		case ir::Opcodes::Cmp:
			stream << "cmp" << "\n";
			break;
		case ir::Opcodes::Je:
			stream << "je" << "\n";
			break;
		case ir::Opcodes::Jne:
			stream << "jne" << "\n";
			break;
		default:
			break;
		}
	}

	return stream.str();
}

std::string getValue(ir::Value* value) {
	std::stringstream str;
	switch (value->type)
	{
	case ir::Types::Int32:
		push(&str, "mov eax, " + std::to_string(((ir::Number*) value)->Value));
		push(&str, "push eax");
		break;
	case ir::Types::VPointer:
		push(&str, "mov eax,  DWORD [ebp - " + std::to_string(((ir::VariablePointer*) value)->pointing + 4) + "]");
		push(&str, "push eax");
		break;
	default:
		break;
	}

	return str.str();
}
/*
std::string convertPointer() {
	return std::null;
}
*/
