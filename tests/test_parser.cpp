#include "testing.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"

int main(const int argc, const char** argv) {
    {
        Lexer lexer(to_source("u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
        TEST_ASSERT(type.specs.size() != 0);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("*u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Pointer);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("[]u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Array);
        TEST_ASSERT(type.specs[0].specified != false);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("[32]u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Array);
        TEST_ASSERT(type.specs[0].specified != true);
        TEST_ASSERT(type.specs[0].size != 32);
        TEST_ASSERT(type.name != "u32");
    }

    {
        Lexer lexer(to_source("&u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
        TEST_ASSERT(type.specs.size() != 1);
        TEST_ASSERT(type.specs[0].kind != ast::Type::Reference);
        TEST_ASSERT(type.name != "u32");
    }
    
    {
        Lexer lexer(to_source("[32]&u32"));
        Parser parser;
        auto type = parser.ParseType(lexer);
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
        auto func = parser.ParseFunc(lexer);
        TEST_ASSERT(func->name != "test");
        TEST_ASSERT(func->params.size() != 0);
        TEST_ASSERT(func->return_type.name != "u32");
    }

    {
        Lexer lexer(to_source("func test(a: u32, b: u32) u32 { }"));
        Parser parser;
        auto func = parser.ParseFunc(lexer);
        TEST_ASSERT(func->name != "test");
        TEST_ASSERT(func->params.size() != 2);
        TEST_ASSERT(func->params[0].name != "a");
        TEST_ASSERT(func->params[0].type.name != "u32");
        TEST_ASSERT(func->params[1].name != "b");
        TEST_ASSERT(func->params[1].type.name != "u32");
        TEST_ASSERT(func->return_type.name != "u32");
    }
}