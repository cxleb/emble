#include "ir.h"
#include "stdio.h"
namespace ir{
    
    void add_func_to_prog(prog* _prog, func* func_to_add)
    {
        if(_prog->funcs == 0)
        {
            _prog->funcs = func_to_add;
            return;
        }
        func* last_func;
        for(func* next_func = _prog->funcs; next_func;)
        {
            last_func = next_func;
            next_func = last_func->next;
        }
        last_func->next = func_to_add;
    }

    void add_extern_func_to_prog(prog* _prog, std::string name)
    {
        extern_func* _extern_func = (extern_func*) calloc(1, sizeof(extern_func));
        _extern_func->name = (char*) calloc(1, name.size() + 1);
        strcpy(_extern_func->name, name.data());

        if(_prog->extern_funcs == 0)
        {
            _prog->extern_funcs = _extern_func;
            return;
        }
        extern_func* last;
        for(extern_func* next = _prog->extern_funcs; next;)
        {
            last = next;
            next = last->next;
        }
        last->next = _extern_func;
    }

    index request_local(func* func_to_size, index size)
    {
        func_to_size->locals += size;
        return func_to_size->locals;
    }

    void request_block(func* _func, block* _block)
    {
        _block->number = _func->block_count++;
        if(_func->blocks == 0)
        {
            _func->blocks = _block;
            return;
        }
        block* last_block;
        for(block* next_block = _func->blocks; next_block;)
        {
            last_block = next_block;
            next_block = last_block->next;
        }
        last_block->next = _block;
    }

    void add_value_to_block(block* _block, value* val)
    {
        if(_block->values == 0)
        {
            _block->values = val;
            return;
        }
        value* last_value;
        for(value* next_value = _block->values; next_value;)
        {
            last_value = next_value;
            next_value = next_value->next;
        }
        last_value->next = val;
    }
    
    void print_block(block* print_block)
    {
        for(value* next_value = print_block->values; next_value; next_value = next_value->next)
        {
            printf("%d\n", next_value->type);
        }
    }
    
    void clean_up_block(block* dirty_block)
    {
        for(value* next_value = dirty_block->values; next_value;)
        {
            value* next_next = next_value->next;
            free(next_value);
            next_value = next_next;
        }
    }
    
    value* create_custom(value_instruction instruction, value_type type)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = type;
        val->instruction = instruction;        
        return val;
    }
    
    value* create_custom(value_instruction instruction, value_type type, value_t the_value)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = type;
        val->instruction = instruction; 
        val->the_value.number = the_value;       
        return val;
    }

    value* create_const(value_type type, value_t v)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = type;
        val->instruction = value_instruction::i_const;
        val->the_value.number = v;
        return val;
    }

    value* create_global_const(const char* global_name)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = value_type::global_t;
        val->instruction = value_instruction::i_const;
        val->the_value.string = global_name;
        return val;
    }

    value* create_call(const char* func_name)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = value_type::void_t;
        val->instruction = value_instruction::i_call;
        val->the_value.string = func_name;
        return val;
    }
    value* create_get_local(value_type type, value_t offset)
    {
        value* val = (value*) calloc(1, sizeof(value));
        val->type = type;
        val->instruction = value_instruction::i_get; 
        val->the_value.number = offset;       
        return val;
    }
}