#include "tree_sitter/api.h"
#include "ir.h"
#include "utils.h"

ref<Module> ir_gen(TSTree* tree, const std::vector<char>& source);