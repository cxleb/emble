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
    D(CallStmt) \
    D(BLOCK)
    
#define EXPR_NODES(D) \
    D(Binary) \
    D(Unary) \
    D(Call) \
    D(Integer) \
    D(Float) \
    D(String) \
    D(Identifier)

namespace ast {

class Node {
public:
    // The underlying token, can be used to get the source locations
    Token token;
private:
};

class Type {
public:
    enum Kind {
        Pointer,
        Reference,
        Array
    };
    struct Spec {
        Kind kind;
        // used when the type is Kind::Array
        bool specified; 
        uint64_t size;
    };

    std::vector<Spec> specs;
    std::string name;
    bool is_unknown;
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

// Statements

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

class Block : public Stmt {
public:
    std::vector<ref<Stmt>> stmts;
};

// Expressions

class Binary : public Stmt {
    ref<Expr> lhs;
    ref<Expr> rhs;
};

class Unary : public Stmt {
    ref<Expr> expr;
};

class CallExpr : public Expr {
    Call call;
};

struct Parameter {
    std::string name;
    Type type;
};

class Func : public Node {
public:
    std::string name;
    bool exported;
    std::vector<Parameter> params;
    Type return_type;
    ref<Node> root;
};

class Module : public Node {
public:
    std::vector<ref<Func>> funcs;
};

}