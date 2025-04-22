#pragma once

#include "ast.h"
#include "lexer.h"
#include "utils.h"

class Parser {
public:
    ref<ast::Module> parse_file(std::vector<char>&& source);
    ref<ast::Func> parse_func(Lexer& lexer);
    ref<ast::Stmt> parse_stmt(Lexer& lexer);
    ref<ast::Stmt> parse_if(Lexer& lexer);
    ref<ast::Stmt> parse_for(Lexer& lexer);
    ref<ast::Stmt> parse_while(Lexer& lexer);
    ref<ast::Stmt> parse_return(Lexer& lexer);
    ref<ast::Stmt> parse_block(Lexer& lexer);
    ref<ast::Stmt> parse_var(Lexer& lexer);
    ref<ast::Stmt> parse_assign(Lexer& lexer);
    ref<ast::Expr> parse_expr(Lexer& lexer);
    ref<ast::Expr> parse_primary_expr(Lexer& lexer);
    ref<ast::Expr> parse_ident(Lexer& lexer);
    ref<ast::Expr> parse_number(Lexer& lexer);
    ref<ast::Expr> parse_string(Lexer& lexer);
    u8 parse_prec(Token token);
    ast::BinaryExpr::Kind parse_bin_op_kind(Token token);
    ref<ast::Expr> parse_bin_expr(Lexer& lexer, u8 prec);
    ast::Type parse_type(Lexer& lexer);
};

void parser_error [[noreturn]] (Token token, const char* message, ...);
