#pragma once
#include "utils.h"
#include <string>
#include <vector>
#include <variant>
#include <optional>

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
// template<class... Ts>
// overloaded(Ts...) -> overloaded<Ts...>;

using Ref = uint64_t;

enum TypeType {
    TypePointer,
    TypeArray,
    TypeReference,
    TypeIdentifier,
    TypeUnknown,
    TypeInt8,
    TypeInt16,
    TypeInt32,
    TypeInt64,
    TypeUInt8,
    TypeUInt16,
    TypeUInt32,
    TypeUInt64,
    TypeFloat,
    TypeDouble,
};

struct Type {
    TypeType type;
    std::variant<ref<Type>, std::string> inner;
    std::optional<uint32_t> count; // used for array types
};

struct Block;
struct Func;

struct Variable {
    uint64_t number;
    std::string name;
    ref<Type> type;
    bool is_const;
};

struct DataNull {};
struct DataBlock {
    ref<Block> block;
};
struct DataCall{
	std::string name;
	std::vector<Ref> params;
};
struct DataIf {
    Ref condition;
    ref<Block> then;
    std::optional<ref<Block>> else_;
};
struct DataVar {
    ref<Variable> var;
    Ref equals;
};
struct DataStore {
    ref<Variable> var;
    Ref equals;
};
struct DataLoad  {
    ref<Variable> var;
};
struct DataUnary {
    Ref value;
};
struct DataBinOp {
    Ref lhs;
    Ref rhs;
};
struct DataInt {
    uint64_t value;
};

using Data = std::variant<
    DataNull,
    DataBlock,
    DataCall,
    DataIf,
    DataVar,
    DataStore,
    DataLoad,
    DataUnary,
    DataBinOp,
    DataInt
>;

enum InstType {
    InstBlock,
    InstCall,
    InstRetVal,
    InstRet,
    InstIf,
    InstVar,
    InstStore,
    InstLoad,
    InstAdd,
    InstEq,
    InstNotEq,
    InstInt
};

struct Inst {
    InstType type;
    Data data;
};

struct Block {
    uint64_t number; 
    Func* func;
    Block* parent;
    std::vector<ref<Variable>> vars;
    std::vector<Inst> insts;

    // Block Methods
    ref<Block> new_child_block();
    ref<Variable> find_variable(const std::string& name);
    Ref add_inst(InstType type, Data data);

    // Builder Methods
    Ref block(ref<Block> block);
    Ref call(std::string name, std::vector<Ref> params);
    Ref ret();
    Ref retval(Ref value);
    Ref if_(Ref condition, ref<Block> then, std::optional<ref<Block>> else_);
    Ref var(const std::string& name, ref<Type> type, bool is_const, Ref equals);
    Ref store(const std::string&, Ref value);
    Ref load(std::string name);
    Ref add(Ref lhs, Ref rhs);
    Ref eq(Ref lhs, Ref rhs);
    Ref noteq(Ref lhs, Ref rhs);
    Ref int_(uint64_t value);
};

struct Func {
    std::string name;
    bool is_exported;
    std::vector<ref<Variable>> params;    
    ref<Block> root;
    uint64_t block_count;
    uint64_t var_count;
    ref<Type> return_type;
};

struct Module {
    //std::vector<ref<ExternFunc>> extern_funcs;
    std::vector<ref<Func>> funcs;
};

void print_module(ref<Module> module); 