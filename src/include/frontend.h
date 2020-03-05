#pragma once

#include <string>
#include "ast.h"

enum token_type{
    identifier,
    number,
    comment,
    string,
    
    // ops
    addition,
    subtraction,
    divison,
    multilication,
    equals,
    right_paren,
    left_paren,
    right_curly,
    left_curly,
    right_bracket,
    left_bracket,
    colon,
    comma,
    
    //keywords -- this really annoys me but because extern, if and else are reserved keywords i put kw_ at the front
    kw_extern,
    kw_func,
    kw_if,
    kw_else,
    kw_elseif,
    kw_var,
    kw_return,
};

struct token{
    
    token_type type;
    std::string text;
    int line;
    
};

namespace frontend{
    struct compiler_state{
        ast::root_node* root;
    };
    
    
    token* get_next_token(std::string &code);
    void parse(std::string &str, compiler_state* state);
    void type_check(compiler_state* state);
    
}

