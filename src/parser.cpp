#include "parser.h"
#include "error.h"

ref<ast::Module> Parser::ParseFile(std::vector<char>&& source) {
    auto lexer = Lexer(std::move(source));
    
    auto module = make_ref<ast::Module>();

    auto token = lexer.next();
    while(token.kind != TokenEndOfFile) {
        bool exported = lexer.test("export");
        if (exported) {
            lexer.next();
        }

        if (lexer.test("func")) {
            auto func = ParseFunc(lexer);
            func->exported = exported;
            module->funcs.push_back(func);
        }
    }

    return module;
}

ref<ast::Func> Parser::ParseFunc(Lexer& lexer) {
    assert(lexer.test("func"));
    lexer.expect(TokenIdentifier);
    auto func = make_ref<ast::Func>();
    func->name = lexer.token_to_string(lexer.expect(TokenIdentifier));
    lexer.expect(TokenLeftParen);
    auto token = lexer.peek();
    while (token.kind != TokenRightParen) {
        ast::Parameter param;
        param.name = lexer.token_to_string(lexer.expect(TokenIdentifier));
        lexer.expect(TokenColon);
        param.type = ParseType(lexer);
        func->params.push_back(param);
        token = lexer.peek();
        if (token.kind == TokenComma) {
            lexer.next();
            token = lexer.peek();
        }
    }
    lexer.expect(TokenRightParen);
    func->return_type = ParseType(lexer);
    func->root = ParseBlock(lexer);
    
    return func;
}

ref<ast::Stmt> Parser::ParseStmt(Lexer& lexer) {
    auto stmt = make_ref<ast::Stmt>();
    return stmt;
}

ref<ast::Expr> Parser::ParseExpr(Lexer& lexer) {
    auto expr = make_ref<ast::Expr>();
    return expr;
}

ref<ast::Stmt> Parser::ParseBlock(Lexer& lexer) {
    lexer.expect(TokenLeftCurly);
    lexer.expect(TokenRightCurly);
    auto stmt = make_ref<ast::Stmt>();
    return stmt;
}

ast::Type Parser::ParseType(Lexer& lexer) {
    ast::Type type;
    type.is_unknown = false;
    Token token;

    bool working = true;
    while (working)
    {
        token = lexer.next();
        uint64_t size = 0;
        bool specified = false;
        switch(token.kind) {
        case TokenAstericks:
            type.specs.push_back(ast::Type::Spec{
                .kind = ast::Type::Pointer,
            });
            break;
        case TokenAmpersand:
            type.specs.push_back(ast::Type::Spec{
                .kind = ast::Type::Reference,
            });
            break;
        case TokenLeftBracket:
            token = lexer.next();
            if (token.kind == TokenNumber) {
                size = lexer.token_to_int(token);
                token = lexer.next();
                specified = true;
            }
            if (token.kind != TokenRightBracket) {
                parser_error(token, "For array type, you need close with ']'");
            }            
            type.specs.push_back(ast::Type::Spec{
                .kind = ast::Type::Array,
                .specified = specified,
                .size = size,
            });
            break;
        default:
            working = false;
            break;
        }
    }

    if (token.kind == TokenIdentifier) {
        type.name = lexer.token_to_string(token);
    } else {
        parser_error(token, "Expected identifier when defining a type");
    }

    return type;
}