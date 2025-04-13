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

Ref Block::add_inst(InstType type, Data data) {
    Ref r = insts.size();
    insts.push_back(Inst {
        type,
        data
    });
    return r;
}

Ref Block::block(ref<Block> block) {
    return add_inst(InstBlock, DataBlock{
        block
    });
}

Ref Block::call(std::string name, std::vector<Ref> params) {
    return add_inst(InstCall, DataCall{
        name,
        params,
    });
}

Ref Block::ret() {
    return add_inst(InstRet, DataNull{});
}

Ref Block::retval(Ref value) {
    return add_inst(InstRetVal, DataUnary{
        value
    });
}

Ref Block::if_(Ref condition, ref<Block> then, std::optional<ref<Block>> else_) {
    return add_inst(InstIf, DataIf{
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
    var->number = func->var_count++;
    vars.push_back(var);
    return add_inst(InstVar, DataVar{
        var,
        equals
    });
}

Ref Block::store(const std::string& name, Ref value) {
    auto var = find_variable(name);
    if (var == nullptr) {
        // TODO(caleb): compiler error
    }
    return add_inst(InstStore, DataStore{
        var,
        value
    });
}

Ref Block::load(std::string name) {
    auto var = find_variable(name);
    if (var == nullptr) {
        // TODO(caleb): compiler error
    }
    return add_inst(InstLoad, DataLoad{
        var
    });
}

Ref Block::add(Ref lhs, Ref rhs) {
    return add_inst(InstAdd, DataBinOp{
        lhs,
        rhs
    });
}

Ref Block::eq(Ref lhs, Ref rhs) {
    return add_inst(InstEq, DataBinOp{
        lhs,
        rhs
    });
}

Ref Block::noteq(Ref lhs, Ref rhs) {
    return add_inst(InstNotEq, DataBinOp{
        lhs,
        rhs
    });
}

Ref Block::int_(uint64_t value) {
    return add_inst(InstInt, DataInt{
        value
    });    
}

void print_indent(uint32_t indent) {
    for(uint32_t i = 0; i < indent; i++) {
        printf("  ");
    }
}

void print_block(ref<Block> b, uint32_t indent) {
    print_indent(indent);
    printf("Block %lld\n", b->number);
    for(uint32_t i = 0; i < b->insts.size(); i++) {
        Inst& inst = b->insts[i];
        print_indent(indent);
        printf("%d: ", i);
        switch(inst.type) {
        case InstBlock: {
            printf("Block: \n");
            auto data = std::get<DataBlock>(inst.data);
            print_block(data.block, indent + 1);
            break;
        }
        case InstCall: {
            auto data = std::get<DataCall>(inst.data);
            printf("Call %s (n %lld)\n", data.name.c_str(), data.params.size());
            break;
        }
        case InstRetVal: {
            auto data = std::get<DataUnary>(inst.data);
            printf("Ret %lld\n", data.value);
            break;
        }
        case InstRet: {
            printf("Ret\n");
            break;
        }
        case InstIf: {
            auto data = std::get<DataIf>(inst.data);
            printf("If %lld\n", data.condition);
            print_indent(indent);
            printf("then:\n");
            print_block(data.then, indent + 1);
            if (auto else_ = data.else_) {
                print_indent(indent);
                printf("else:\n");
                print_block(*else_, indent + 1);
            }
            break;
        }
        case InstVar: {
            auto data = std::get<DataVar>(inst.data);
            printf("Var %s: %s = %lld\n", data.var->name.c_str(), "unknown", data.equals);
            break;
        }
        case InstStore: {
            auto data = std::get<DataStore>(inst.data);
            printf("Store %s: %s = %lld\n", data.var->name.c_str(), "unknown", data.equals);
            break;
        }
        case InstLoad: {
            auto data = std::get<DataLoad>(inst.data);
            printf("Load %s: %s\n", data.var->name.c_str(), "unknown");
            break;
        }
        case InstAdd: {
            auto data = std::get<DataBinOp>(inst.data);
            printf("Add %lld %lld\n", data.rhs, data.lhs);
            break;
        }
        case InstEq: {
            auto data = std::get<DataBinOp>(inst.data);
            printf("Eq %lld %lld\n", data.rhs, data.lhs);
            break;
        }
        case InstNotEq: {
            auto data = std::get<DataBinOp>(inst.data);
            printf("NotEq %lld %lld\n", data.rhs, data.lhs);
            break;
        }
        case InstInt: {
            auto data = std::get<DataInt>(inst.data);
            printf("Int %lld\n", data.value);
            break;
        }
        }
    }
}

void print_func(ref<Func> func) {
    printf("func %s (n %lld)\n", func->name.c_str(), func->params.size());
    print_block(func->root, 1);
}

void print_module(ref<Module> module) {
    for(auto func : module->funcs) {
        print_func(func);
    }
}