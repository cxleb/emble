#include "codegen.h"
#include <iostream>
#include <cassert>

namespace codegen
{
// TEMPORARY CODEGEN STATE
	struct scope_mapping
	{
		const char* name;//std::string name;
		value_t offset;
		ir::value_type type;
		scope_mapping* next;
	};

	struct codegen_state
	{
		ir::prog* prog;
		ir::func* current_func;
		ir::block* current_block;
		scope_mapping* scope;
		frontend::func_signature* func_signatures;
		int const_index;
	};

	ir::value_type convert_type(std::string name)
	{
		if (name == "uint8")
			return ir::value_type::uint8;
		else if (name == "int8")
			return ir::value_type::int8;
		else if (name == "uint16")
			return ir::value_type::uint16;
		else if (name == "int16")
			return ir::value_type::int16;
		else if (name == "uint32")
			return ir::value_type::uint32;
		else if (name == "int32")
			return ir::value_type::int32;
		else if (name == "uint64")
			return ir::value_type::uint64;
		else if (name == "int64")
			return ir::value_type::int64;
		else if (name == "void")
			return ir::value_type::void_t;
		else if (name == "cstring")
			return ir::value_type::string_t;
		return ir::value_type::void_t;			
	}

	value_t get_type_size(ir::value_type type)
	{
		switch(type)
		{
			case ir::value_type::uint8:
			case ir::value_type::int8:
				return 1;
			case ir::value_type::uint16:
			case ir::value_type::int16:
				return 2;
			case ir::value_type::uint32:
			case ir::value_type::int32:
			case ir::value_type::global_t: // this is for 32bit systems, for 64bbit put it in 8bytes, but for now i dont understand 100% 64bit asm
				return 4;
			case ir::value_type::uint64:
			case ir::value_type::int64:
			case ir::value_type::string_t:
				return 8;
			default: 
				return 0;
		}
	}

	void add_value_to_scope(codegen_state* state, scope_mapping* mapping)
	{
		//printf("%llu", state->scope);
		if(state->scope == 0)
        {
        	state->scope = mapping;
            return;
        }
        scope_mapping* last;
        for(scope_mapping* next = state->scope; next;)
        {
            last = next;
            next = last->next;
        }
        last->next = mapping;
    }

	scope_mapping* search_scope(codegen_state* state, std::string name)
	{
		scope_mapping* next;
        for(next = state->scope; next; next = next->next)
        {
        	if (!strcmp(next->name, name.c_str()))
            	return next;
        }
        printf("variable %s not found in scope", name.c_str());
        exit(0);
        return nullptr;
	}

	frontend::func_signature* search_function_signatures(codegen_state* state, std::string name)
	{
		frontend::func_signature* next;
		for(next = state->func_signatures; next; next = next->next)
        {
        	if (next->name == name)
            	return next;
        }
        printf("function %s not found", name.c_str());
        exit(0);
        return nullptr;
	}

	void statement_codegen(codegen_state* state, frontend::ast::expression_node* node, ir::value_type marked_type) // what we normally expect in a block
	{
		switch(node->type)
		{
			case frontend::ast::node_type::integer:
			{
				ir::add_value_to_block(state->current_block, ir::create_const(marked_type, ((frontend::ast::integer_node*)node)->value));
			} 
			break;
			case frontend::ast::node_type::string:
			{
				std::string name = "cnst";
				name += std::to_string(state->const_index);
				ir::add_global_const_to_prog(state->prog, name, ir::create_string(((frontend::ast::string_node*)node)->value.data()));
				char* ptr = (char*) calloc(1, name.size() + 1);
        		strcpy(ptr, name.data());
				ir::add_value_to_block(state->current_block, ir::create_global_const(ptr));
			} 
			break;
			case frontend::ast::node_type::return_:
			{
				if(((frontend::ast::return_node*)node)->return_value != 0) // check if there is a return value node
					statement_codegen(state, ((frontend::ast::return_node*)node)->return_value, state->current_func->return_type);
				add_value_to_block(state->current_block, ir::create_custom(ir::value_instruction::i_ret, state->current_func->return_type));
			} 
			break;
			case frontend::ast::node_type::block:
			{
				ir::block* block = new ir::block();
				state->current_block = block;
				ir::request_block(state->current_func, block);
				for(frontend::ast::expression_node* statement : ((frontend::ast::block_node*)node)->statements)
				{
					statement_codegen(state, statement, marked_type);
				}
			} 
			break;
			case frontend::ast::node_type::math:
			{
				statement_codegen(state, ((frontend::ast::math_node*)node)->lhs, marked_type);
				statement_codegen(state, ((frontend::ast::math_node*)node)->rhs, marked_type);
				switch(((frontend::ast::math_node*)node)->op)
				{
					case frontend::ast::math_op::add:
						add_value_to_block(state->current_block, ir::create_custom(ir::value_instruction::i_add, marked_type));
					break;
					default:
					break;
				}
			}
			break;
			case frontend::ast::node_type::variable:
			{
				scope_mapping* mapping = (scope_mapping*) calloc(1, sizeof(scope_mapping));
				mapping->name = ((frontend::ast::variable_node*)node)->name.c_str();
				mapping->type = convert_type(((frontend::ast::variable_node*)node)->variable_type);
				mapping->offset = request_local(state->current_func, get_type_size(mapping->type));
				add_value_to_scope(state, mapping);
				// set the variable
				if (((frontend::ast::variable_node*)node)->assignment)
				{
					statement_codegen(state, ((frontend::ast::variable_node*)node)->assignment, mapping->type);
					add_value_to_block(state->current_block, ir::create_custom(ir::value_instruction::i_set, mapping->type, mapping->offset));
				}
			}
			break;
			case frontend::ast::node_type::variable_reference:
			{
				scope_mapping* mapping = search_scope(state, ((frontend::ast::variable_reference_node*)node)->name);
				add_value_to_block(state->current_block, ir::create_custom(ir::value_instruction::i_get, mapping->type, mapping->offset));
			}
			break;
			case frontend::ast::node_type::func_call:
			{
				frontend::func_signature* signature = search_function_signatures(state, ((frontend::ast::func_call_node*)node)->name);
				frontend::func_parameter* param = signature->parameters;
				value_t counter = 1;
				for(frontend::ast::expression_node* statement : ((frontend::ast::func_call_node*)node)->arguements)
				{
					ir::value_type parameter_type = convert_type(param->type);
					param = param->next;
					statement_codegen(state, statement, parameter_type);
					add_value_to_block(state->current_block, ir::create_custom(ir::value_instruction::i_args, ir::value_type::uint8, counter));
					counter += 1;
				}
				add_value_to_block(state->current_block, ir::create_call(((frontend::ast::func_call_node*)node)->name.c_str()));
			}
			break;
			default:
			{

			} 
			break;
			
		}
	}
	ir::func* codegen_func(codegen_state* state, frontend::ast::func_node* func)
	{
		ir::func* new_func = new ir::func();
		new_func->name = func->name;
		new_func->return_type = convert_type(func->return_type);

		state->current_func = new_func;

		new_func->parameter_count = func->arguements.size();
		for(frontend::ast::expression_node* parameter : func->arguements)
		{
			scope_mapping* mapping = (scope_mapping*) calloc(1, sizeof(scope_mapping));
			switch(parameter->type)
			{
				case frontend::ast::node_type::variable:
					mapping->name = ((frontend::ast::variable_node*)parameter)->name.c_str();
					mapping->type = convert_type(((frontend::ast::variable_node*)parameter)->variable_type);
					mapping->offset = request_local(state->current_func, 8);
				break;
				default:
					printf("please declare variables in a function");
					exit(0);
				break;
			}
			add_value_to_scope(state, mapping);
		}
		
		statement_codegen(state, func->statement, new_func->return_type);
		
		return new_func;
	}

	ir::prog* codegen(frontend::compiler_state* state)
	{
		ir::prog* prog = new ir::prog();

		codegen_state* cgstate = (codegen_state*)calloc(1, sizeof(codegen_state));//new codegen_state();
		cgstate->func_signatures = state->func_signatures;
		cgstate->prog = prog;

		for(frontend::ast::expression_node* func : state->root->functions)
		{
			if(func->type == frontend::ast::node_type::func)
			{
				ir::func* new_func = codegen_func(cgstate, (frontend::ast::func_node*)func);
				ir::add_func_to_prog(prog, new_func);
			}
			else if(func->type == frontend::ast::node_type::extern_func)
			{
				ir::add_extern_func_to_prog(prog, ((frontend::ast::extern_func_node*)func)->name);
			}
		}

		return prog;
	}
}