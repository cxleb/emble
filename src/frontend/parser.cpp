#include "frontend.h"
#include "ast.h"

#include <iostream>
#include <queue>
#include <cstdarg>

using namespace frontend::ast;

namespace frontend {
    static std::queue <token*> tokens;
    
    extern_func_node* extern_func_parser();
    func_node* func_parser();
    expression_node* expression_parser();
    expression_node* return_parser();
    expression_node* string_parser();
    expression_node* integer_parser();
    expression_node* identifier_parser();
    expression_node* variable_parser();
    expression_node* statement_parser();
    expression_node* block_parser();
    
    // copied from flax lang parser, modified for use in emble
    void error(const char* msg, ...)
    {
        va_list ap;
        va_start(ap, msg);
        
        char alloc[100];
        vsprintf(alloc, msg, ap);
        
        fprintf(stderr, "Error:%d: %s\n\n",tokens.front()->line, alloc);
        
        va_end(ap);
        exit(1);
    }
    
    static int get_op()
    {
        token* tok = tokens.front();
        switch (tok->type)
        {
            case token_type::multilication:
            return math_op::mult;
            case token_type::divison:
            return math_op::div;
            case token_type::addition:
            return math_op::add;
            case token_type::subtraction:
            return math_op::sub;
            
            default:
            return -1;
        }
    }
    
    // main bits of the parser
    
    void parse(std::string &src, compiler_state* state) {
        token* tok;
        state->root = new root_node();
        state->root->type = node_type::root;
        
        while ((tok = frontend::get_next_token(src)) != nullptr) {
            //std::cout << token->line << " '" << token->text << "'" << std::endl;
            if (tok->type != token_type::comment)
                tokens.push(tok);
        }
        
        while (!tokens.empty()) {
            tok = tokens.front();
            if (tok->type == token_type::kw_func) {
                state->root->functions.push_back(func_parser());
            }
            else if (tok->type == token_type::kw_extern){
                state->root->functions.push_back(extern_func_parser());
            }
            else {
                error("Unkown token: %s", tok->text);
            }
        }
    }
    
    extern_func_node* extern_func_parser(){
        extern_func_node* node = new extern_func_node();
        node->type = node_type::extern_func;
        tokens.pop();
        
        if (tokens.front()->type == token_type::kw_func) {
            tokens.pop();
        }
        else {
            error("Cannot find func in extern function");
        }
        
        if (tokens.front()->type == token_type::colon) {
            tokens.pop();
            if(tokens.front()->type == token_type::identifier){
                node->return_type = tokens.front()->text;
                tokens.pop();
            }else{
                error("Unexpected Identifier in external function definition");
            }
        }
        
        if (tokens.front()->type == token_type::identifier) {
            node->name = tokens.front()->text;
            tokens.pop();
        }
        else {
            error("Expected Name in function declaration");
        }
        
        if (tokens.front()->type == token_type::left_paren) {
            tokens.pop();
            while (tokens.front()->type != token_type::right_paren) {
                node->arguements.push_back(expression_parser());
                if (tokens.front()->type != token_type::right_paren && tokens.front()->type != token_type::comma) {
                    std::cout << tokens.front()->text << std::endl;
                    error("Expected ',' or ')' in function declaration");
                }
                if (tokens.front()->type == token_type::comma) {
                    tokens.pop();
                }
            }
            tokens.pop();
        }
        else {
            error("Expected '(' in function declaration");
        }
        
        return node;
    }
    
    func_node* func_parser() {
        // assumed that we know the next token is a func token
        tokens.pop();
        func_node* func = new func_node();
        func->type = node_type::func;
        
        if (tokens.front()->type == token_type::colon) {
            tokens.pop();
            if(tokens.front()->type == token_type::identifier){
                func->return_type = tokens.front()->text;
                tokens.pop();
            }else{
                error("Unexpected Identifier in external function definition");
            }
        }
        
        if (tokens.front()->type == token_type::identifier){
            func->name = tokens.front()->text;
            tokens.pop();
        }
        else {
            error("Expected Name in function declaration");
        }
        
        if (tokens.front()->type == token_type::left_paren) {
            tokens.pop();
            while (tokens.front()->type != token_type::right_paren) {
                func->arguements.push_back(expression_parser());
                if (tokens.front()->type != token_type::right_paren && tokens.front()->type != token_type::comma) {
                    std::cout << tokens.front()->text << std::endl;
                    error("Expected ',' or ')' in function declaration");
                }
                if (tokens.front()->type == token_type::comma) {
                    tokens.pop();
                }
            }
            tokens.pop();
        }
        else {
            error("Expected '(' in function declaration");
        }
        
        func->statement = expression_parser();
        
        return func;
    }
    
    //in function parser part
    
    expression_node* expression_parser() {
        expression_node* expr = statement_parser();
        int op = get_op();
        
        if (op == -1)
            return expr;
        
        tokens.pop();
        
        math_node* math = new math_node();
        math->type = node_type::math;
        math->lhs = expr;
        math->op = op;
        math->rhs = expression_parser();
        return math;
    }
    
    expression_node* statement_parser() {
        //std::cout << tokens.front()->text << " " << tokens.front()->type << std::endl;
        switch (tokens.front()->type) {
            case token_type::identifier:
            return identifier_parser();
            case token_type::number:
            return integer_parser();
            //case token_type::kw_var:
            //return variable_parser();
            //break;
            case token_type::string:
            return string_parser();
            case token_type::left_curly:
            return block_parser();
            case token_type::kw_return:
            return return_parser();
            default:
            break;
        }
        tokens.pop();
        return nullptr;
    }

    expression_node* return_parser()
    {
        tokens.pop();
        return_node* node = new return_node();
        node->type = node_type::return_;

        node->return_value = expression_parser();

        return node;
    }

    expression_node* string_parser()
    {
        string_node* node = new string_node();
        node->type = node_type::string;

        node->value = tokens.front()->text;
        tokens.pop();

        return node;
    }
    
    expression_node* integer_parser() {
        integer_node* node = new integer_node();
        node->type = node_type::integer;
        
        node->value = (int64_t)std::stoll(tokens.front()->text);
        tokens.pop();
        
        return node;
    }
    
    expression_node* identifier_parser() {
        token* name = tokens.front();
        tokens.pop();
        
        if (tokens.front()->type == token_type::left_paren) {
            tokens.pop();
            func_call_node* func = new func_call_node();
            func->type = node_type::func_call;
            func->name = name->text;
            
            while (tokens.front()->type != token_type::right_paren) {
                func->arguements.push_back(expression_parser());
                if (tokens.front()->type != token_type::right_paren && tokens.front()->type != token_type::comma) {
                    error("Expected ',' or ')' in function call");
                }
                if (tokens.front()->type == token_type::comma) {
                    tokens.pop();
                }
            }
            tokens.pop();
            return func;
        }
        else if(tokens.front()->type == token_type::colon)
        {
            tokens.pop();
            variable_node* var = new variable_node();
            var->type = node_type::variable;
            var->name = name->text;
            
            if(tokens.front()->type == token_type::identifier)
            {
                var->variable_type = tokens.front()->text;
                tokens.pop();
            }
            
            if(tokens.front()->type == token_type::equals)
            {
                tokens.pop();
                var->assignment = expression_parser();
            }
            /*else
            {
                error("Variable Assignment missing value");
            }*/
            
            return var;
        }
        
        variable_reference_node* node = new variable_reference_node();
        node->type = node_type::variable_reference;
        node->name = name->text;
        return node;
    }
    
    expression_node* variable_parser() { // TODO ADD VARIABLES AND PRINT VARIABLE
        // assumed that we know the next token is a var token
        tokens.pop();
        variable_node* var = new variable_node();
        var->type = node_type::variable;
        
        std::string buffer;
        
        if(tokens.front()->type == token_type::identifier){
            //var->type = tokens.front()->text;
            buffer = tokens.front()->text;
            tokens.pop();
        }else{
            error("Unexpected token in variable definition");
        }
        
        if (tokens.front()->type == token_type::identifier) {
            var->name = tokens.front()->text;
            var->type = node_type::variable;
            tokens.pop();
        }
        else {
            var->name = buffer;
        }
        
        if (tokens.front()->type == token_type::equals) {
            tokens.pop();
            var->assignment = expression_parser();
        }
        else {
            var->assignment = nullptr;
        }
        
        return var;
    }
    
    expression_node* block_parser() {
        tokens.pop();
        block_node* node = new block_node();
        node->type = node_type::block;
        
        while (tokens.front()->type != token_type::right_curly) {
            node->statements.push_back( expression_parser() );
        }
        tokens.pop();
        
        return node;
    }
    
}
