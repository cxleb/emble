#pragma once

#include "ast.h"
#include "lexer.h"
#include "utils.h"

class Parser {
public:
    ref<ast::Module> ParseFile(std::vector<char>&& source);
    ref<ast::Func> ParseFunc(Lexer& lexer);
    ref<ast::Stmt> ParseStmt(Lexer& lexer);
    ref<ast::Stmt> ParseBlock(Lexer& lexer);
    ref<ast::Expr> ParseExpr(Lexer& lexer);
    ast::Type ParseType(Lexer& lexer);
};