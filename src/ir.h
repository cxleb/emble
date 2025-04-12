#pragma once
#include "utils.h"
#include <string>
#include <vector>
#include <variant>
#include <optional>

using Ref = uint64_t;

struct Type {

};

struct Block;
struct Func;

struct Variable {
    std::string name;
    Type type;
    bool is_const;
};

struct InstBlock {
    ref<Block> block;
};
struct InstRet {
    std::optional<Ref> value;
};

struct InstIf {
    Ref condition;
    ref<Block> then;
    std::optional<ref<Block>> else_;
};
struct InstVar {
    ref<Variable> var;
    Ref equals;
};
struct InstStore {
    ref<Variable> var;
    Ref equals;
};
struct InstLoad  {
    ref<Variable> var;
};
struct InstAdd {
    Ref lhs;
    Ref rhs;
};
struct InstEq {
    Ref lhs;
    Ref rhs;
};
struct InstNotEq {
    Ref lhs;
    Ref rhs;
};

using Inst = std::variant<
    InstBlock,
    InstRet,
    InstIf,
    InstVar,
    InstStore,
    InstLoad,
    InstAdd,
    InstEq,
    InstNotEq
>;

struct Block {
    uint64_t number; 
    Func* func;
    Block* parent;
    std::vector<ref<Variable>> vars;
    std::vector<Inst> insts;

    // Block Methods
    ref<Block> new_child_block();
    ref<Variable> find_variable(const std::string& name);
    Ref add_inst(Inst inst);

    // Builder Methods
    Ref block(ref<Block> block);
    Ref ret(std::optional<Ref> value);
    Ref if_(Ref condition, ref<Block> then, std::optional<ref<Block>> else_);
    Ref var(const std::string& name, Type type, bool is_const, Ref equals);
    Ref store(const std::string&, Ref value);
    Ref load(std::string name);
    Ref add(Ref lhs, Ref rhs);
    Ref eq(Ref lhs, Ref rhs);
    Ref noteq(Ref lhs, Ref rhs);
};

struct Func {
    std::string name;
    std::vector<ref<Variable>> params;    
    ref<Block> root;
    uint64_t block_count;
};

struct Module {
    //std::vector<ref<ExternFunc>> extern_funcs;
    std::vector<ref<Func>> funcs;
};