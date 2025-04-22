#include "tree_sitter/api.h"
#include "ir.h"
#include "utils.h"

namespace ir {

ref<Module> ir_gen(TSTree* tree, const std::vector<char>& source);

}