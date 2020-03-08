#include "frontend.h"
#include "ast.h"
#include <map>

namespace frontend {
    /*
    func/extern
    variable
    static std::map<std::string, std::string> typeMatching;
    
    the type checker is currently going to be implemented in 2
    
    first pass is to gather types, structs, func definitions, etc
    second pass is go through each function and ensure 
        we arent using any invalid types and to do type 
        inference for funcs and variables(the latter will be 
        implemeneted later)


*/
    void add_parameter(func_signature* signature,  func_parameter* param)
    {
        if(signature->parameters == 0)
        {
            signature->parameters = param;
            return;
        }
        func_parameter* last;
        for(func_parameter* next = signature->parameters; next;)
        {
            last = next;
            next = last->next;
        }
        last->next = param;
    }

    void add_signature(compiler_state* state, func_signature* signature)
    {
        if(state->func_signatures == 0)
        {
            state->func_signatures = signature;
            return;
        }
        func_signature* last;
        for(func_signature* next = state->func_signatures; next;)
        {
            last = next;
            next = last->next;
        }
        last->next = signature;
    }


    void function_checker(ast::func_node* node);
    void extern_func_checker(ast::extern_func_node* node);
    
    void type_check(compiler_state* state){
        //first pass
        for (ast::expression_node* e : state->root->functions)
        {
            switch(e->type){
                case ast::node_type::func:
                {
                    ast::func_node* node = (ast::func_node*)e;
                    func_signature* signature = new func_signature();
                    signature->name = node->name;
                    signature->return_type = node->return_type;
                    for (ast::expression_node* parameter : node->arguements)
                    {
                        func_parameter* param = new func_parameter();
                        if (parameter->type == ast::node_type::variable)
                        {
                            param->name = ((ast::variable_node*)parameter)->name;
                            param->type = ((ast::variable_node*)parameter)->variable_type;
                        }
                        else
                        {
                            printf("error in function thingo");
                            exit(0);
                        }
                        add_parameter(signature, param);
                    }
                    add_signature(state, signature);
                }   
                break;
                case ast::node_type::extern_func:
                break;
                default:
                break;
            }
        }

        for (func_signature* next = state->func_signatures; next; next = next->next)
        {
            printf("name: %s return type: %s\n", next->name.c_str(), next->return_type.c_str());
            printf("parameters:\n");
            for(func_parameter* nextp = next->parameters; nextp; nextp = nextp->next)
            {
                printf("\tname: %s return type: %s\n", nextp->name.c_str(), nextp->type.c_str());
            }
        }

        // TODO: implement
        /*
        for (ast::expression_node* e : state->root->functions)
        {
            switch(e->type){
                case ast::node_type::func:
                function_checker((ast::func_node*)e);
                break;
                case ast::node_type::extern_func:
                extern_func_checker((ast::extern_func_node*)e);
                break;
                default:
                printf("yeah%d\n", e->type);
                break;
            }
        }
        */
    }
    
    void function_checker(ast::func_node* node){
        if(node->return_type == ""){
            node->return_type = "void";
        }
    }
    
    void extern_func_checker(ast::extern_func_node* node){
        if(node->return_type == ""){
            node->return_type = "void";
        }
    }
    
    void expression_checker(ast::expression_node* node){
        switch(node->type){
            case ast::node_type::variable:
            {
                node = (ast::variable_node*)node;
                if(node->type == NULL)
                {
                    
                }
            }
				break;
            default:
				break;
        }
    }
}

