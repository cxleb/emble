#include "frontend.h"
#include "ast.h"
#include <map>

namespace frontend {
    /*
    func/extern
    variable
    static std::map<std::string, std::string> typeMatching;
*/
    
    
    void function_checker(ast::func_node* node);
    void extern_func_checker(ast::extern_func_node* node);
    
    void type_check(compiler_state* state){
        // TODO: implement
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

