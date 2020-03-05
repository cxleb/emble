#pragma once

#include "ir.h"

namespace backend
{
	std::string translate_prog(ir::prog* prog);
	std::string translate_func(ir::func* func);
    std::string translate_block(ir::block* block, std::string func_name);
}