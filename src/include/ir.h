#pragma once
#include <deque>
#include <map>
#include <string>

#define index unsigned int
#define value_t unsigned long long int

namespace ir
{
    enum value_instruction
    {
        //instructions
        i_const,
        i_get,
        i_set,

        i_add,
        i_sub,
        i_mul,
        i_div,
        
        i_call,
        i_jmp,
        i_cmp,
        i_je,
        i_jne,
        i_ret,
        
        i_local_to_global,
    };

    enum value_type
    {
        void_t,
        local_t,
        global_t, // emble has no global, for safety, but it does have constant locals which can be accessed globalably

        uint8,
        uint16,
        uint32,
        uint64,

        int8,
        int16,
        int32,
        int64,

        //string_t,
    };

    union values
    {
        value_t number;
        const char* string;
    };
    
    struct value
    {
        //~value(){}
        value_instruction instruction;
        value_type type;
        values the_value;
        value* next;
    };
    
    struct block
    {
        //~block(){}
        index number;
        value* values;
        block* next;
    };
    
    struct func
    {
        //~func(){}
        std::string name;
        func* next;
        block* blocks;
        index locals;
        index block_count;
        value_type return_type;
    };

    struct prog
    {
        func* funcs;
        value* global_consts;
    };
    
    void add_func_to_prog(prog* _prog, func* func_to_add);
    index request_local(func* func_to_size, index size);
    void request_block(func* _func, block* _block);
    void add_value_to_block(block* add_block, value* val);
    void print_block(block* print_block);
    void clean_up_block(block* dirty_block);
    value* create_custom(value_instruction instruction, value_type type);
    value* create_custom(value_instruction instruction, value_type type, value_t the_value);
    value* create_const(value_type type, value_t v);
    value* create_global_const(const char* global_name);
    value* create_call(const char* func_name);
    value* create_get_local(value_type type, value_t offset);
    
}