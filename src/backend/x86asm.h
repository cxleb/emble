#pragma once

#include <string>
#include "ir/module.h"
#include "ir/function.h"
#include "ir/block.h"

std::string translateModule(ir::module* module);
std::string translateFunction(ir::function* func);
std::string translateBlock(std::string name, int bnum, ir::block* block);
std::string getValue(ir::Value* value);