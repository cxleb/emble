#include "frontend.h"
#include "ast.h"
#include <map>

// so far what this code has to implement
// - check func references are correct
// - check variable types are correct
// - type inference(lata)

namespace frontend {
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
        //scan for func defintions
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
                {
                    ast::extern_func_node* node = (ast::extern_func_node*)e;
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
                default:
                break;
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

