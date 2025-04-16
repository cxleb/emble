#pragma once
#include <string>
#include <vector>
#include <optional>
#include "utils.h"
#include "lexer.h"

#define STMT_NODES(D) \
    D(If) \
    D(Return) \
    D(VarDecl) \
    D(Assign) \
    D(While) \
    D(For) \
    D(CallStmt)
    
#define EXPR_NODES(D) \
    D(Binary) \
    D(Unary) \
    D(Call) \
    D(Integer) \
    D(Float) \
    D(String) \
    D(Identifier)

class Node {
public:
    // The underlying token, can be used to get the source locations
    Token token;

private:
};

class Stmt : public Node {
public:
    
};

class Expr : public Node {
public:
    enum Kind {
        #define NAME(name) Kind##name,
        EXPR_NODES(NAME)
        #undef NAME
    } kind;
};

class If : public Stmt {
public:
    ref<Expr> condition;
    ref<Stmt> then_stmt;
    ref<Stmt> else_stmt;
};

class Return : public Stmt {
public:
    std::optional<ref<Expr>> value;
};

class VarDecl : public Stmt {
    std::string name;
    ref<Expr> value;
};

class Call : public Node {
    std::string name;
    std::vector<ref<Expr>> args;
};

class BinaryExpr : public Stmt {
    ref<Expr> lhs;
    ref<Expr> rhs;
};

class UnaryExpr : public Stmt {
    ref<Expr> expr;
};

class CallExpr : public Expr {
    ref<Call> call;
};



class Func : public Node {
    std::string name;
    ref<Node> root;
};

class Module : public Node {
    std::vector<ref<Func>> funcs;
};