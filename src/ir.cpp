#include "ir.h"

ref<Block> Block::new_child_block() {
    auto block = make_ref<Block>();
    block->number = func->block_count++;
    block->func = func;
    block->parent = this;
    return block;
}

ref<Variable> Block::find_variable(const std::string& name) {
    auto block = this;
    while (block != nullptr) {
        for(auto var: block->vars) {
            if (var->name == name) {
                return var;
            }
        }
        block = block->parent;
    }
    return nullptr;
}

Ref Block::add_inst(Inst inst) {
    Ref r = insts.size();
    insts.push_back(inst);
    return r;
}

Ref Block::block(ref<Block> block) {
    return add_inst(InstBlock{
        block
    });
}

Ref Block::ret(std::optional<Ref> value) {
    return add_inst(InstRet{
        value
    });
}

Ref Block::if_(Ref condition, ref<Block> then, std::optional<ref<Block>> else_) {
    return add_inst(InstIf{
        condition,
        then,
        else_
    });
}

Ref Block::var(const std::string& name, Type type, bool is_const, Ref equals) {
    if (find_variable(name) != nullptr) {
        // TODO(caleb): compiler error
    }
    auto var = make_ref<Variable>();
    var->name = name;
    var->type = type;
    var->is_const = is_const;
    vars.push_back(var);
    return add_inst(InstVar{
        var,
        equals
    });
}

Ref Block::store(const std::string& name, Ref value) {}

Ref Block::load(std::string name) {}

Ref Block::add(Ref lhs, Ref rhs) {}

Ref Block::eq(Ref lhs, Ref rhs) {}

Ref Block::noteq(Ref lhs, Ref rhs) {}