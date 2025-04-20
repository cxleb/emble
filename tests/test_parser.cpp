#include "testing.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include <cstdio>

class TestingVisitor : public ast::Visitor<TestingVisitor> {
public:
    void accept(ref<ast::Expr> expr) {
        printf("Visiting Expr\n");
        // TEST_ASSERT(expr->value != 0);
    }
    void accept(ref<ast::BinaryExpr> expr) {
        printf("Visiting BinaryExpr\n");
        // /TEST_ASSERT(expr->lhs != nullptr);
        // TEST_ASSERT(expr->rhs != nullptr);
    }
};

int main(const int argc, const char** argv) {
    {
        Lexer lexer(to_source("u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 0);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("*u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Pointer);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("[]u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Array);
        TEST_ASSERT(type.specs[0].specified != false);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("[32]u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Array);
        TEST_ASSERT(type.specs[0].specified != true);
        TEST_ASSERT(type.specs[0].size != 32);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("&u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Reference);
        TEST_ASSERT(type.name != "u32");
    }
    
    {
        Lexer lexer(to_source("[32]&u32"));
        Parser parser;
        auto type = parser.parse_type(lexer);
        TEST_ASSERT(type.specs.size() != 2);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Array);
        TEST_ASSERT(type.specs[0].specified != true);
        TEST_ASSERT(type.specs[0].size != 32);
        TEST_ASSERT(type.specs[1].kind != ast::Type::Reference);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("func test() u32 { }"));
        Parser parser;
        auto func = parser.parse_func(lexer);
        TEST_ASSERT(func->name != "test");
        TEST_ASSERT(func->params.size() != 0);
        TEST_ASSERT(func->return_type.name != "u32");
    }

    {
        Lexer lexer(to_source("func test(a: u32, b: u32) u32 { }"));
        Parser parser;
        auto func = parser.parse_func(lexer);
        TEST_ASSERT(func->name != "test");
        TEST_ASSERT(func->params.size() != 2);
        TEST_ASSERT(func->params[0].name != "a");
        TEST_ASSERT(func->params[0].type.name != "u32");
        TEST_ASSERT(func->params[1].name != "b");
        TEST_ASSERT(func->params[1].type.name != "u32");
        TEST_ASSERT(func->return_type.name != "u32");
    }

    // Primary expression tests

    {
        Lexer lexer(to_source("10"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindInteger);
        auto integer = static_ref_cast<ast::Integer>(expr);
        TEST_ASSERT(integer->value != 10);
    }

    {
        Lexer lexer(to_source("ident"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindIdentifier);
        auto integer = static_ref_cast<ast::Identifier>(expr);
        TEST_ASSERT(integer->name != "ident");
    }

    {
        Lexer lexer(to_source("10.10"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindFloat);
        auto integer = static_ref_cast<ast::Float>(expr);
        TEST_ASSERT(integer->value != 10.10);
    }

    {
        Lexer lexer(to_source("\"string\""));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindString);
        auto integer = static_ref_cast<ast::String>(expr);
        TEST_ASSERT(integer->value != "\"string\"");
    }

    // Binary expression tests

    {
        Lexer lexer(to_source("10 + 10"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindBinaryExpr);
        auto bin_expr = static_ref_cast<ast::BinaryExpr>(expr);
        TEST_ASSERT(bin_expr->bin_kind != ast::BinaryExpr::KindAdd);
        TEST_ASSERT(bin_expr->lhs->kind != ast::Expr::KindInteger);
        TEST_ASSERT(bin_expr->rhs->kind != ast::Expr::KindInteger);
    }

    {
        Lexer lexer(to_source("1 + 2 * 3"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindBinaryExpr);
        auto bin_expr = static_ref_cast<ast::BinaryExpr>(expr);
        TEST_ASSERT(bin_expr->bin_kind != ast::BinaryExpr::KindAdd);
        TEST_ASSERT(bin_expr->lhs->kind != ast::Expr::KindInteger);
        TEST_ASSERT(bin_expr->rhs->kind != ast::Expr::KindBinaryExpr);
        auto rhs = static_ref_cast<ast::BinaryExpr>(bin_expr->rhs);
        TEST_ASSERT(rhs->bin_kind != ast::BinaryExpr::KindMultiply);
    }

    {
        Lexer lexer(to_source("1 * 2 + 3"));
        Parser parser;
        auto expr = parser.parse_expr(lexer);
        TEST_ASSERT(expr->kind != ast::Expr::KindBinaryExpr);
        auto bin_expr = static_ref_cast<ast::BinaryExpr>(expr);
        TEST_ASSERT(bin_expr->bin_kind != ast::BinaryExpr::KindAdd);
        TEST_ASSERT(bin_expr->lhs->kind != ast::Expr::KindBinaryExpr);
        TEST_ASSERT(bin_expr->rhs->kind != ast::Expr::KindInteger);
        auto rhs = static_ref_cast<ast::BinaryExpr>(bin_expr->lhs);
        TEST_ASSERT(rhs->bin_kind != ast::BinaryExpr::KindMultiply);
    }
}