#include "parser.h"
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "utils.h"
#include <cstdint>

void parser_error [[noreturn]] (Token token, const char* message, ...) {
    fprintf(stderr, "%llu:%llu: ", token.line, token.col);
    va_list args;
    va_start(args, message);
    verror(message, args);
    va_end(args);     
}

ref<ast::Module> Parser::parse_file(std::vector<char>&& source) {
    auto lexer = Lexer(std::move(source));
    
    auto module = make_ref<ast::Module>();

    auto token = lexer.next();
    while(token.kind != TokenEndOfFile) {
        bool exported = lexer.test("export");
        if (exported) {
            lexer.next();
        }

        if (lexer.test("func")) {
            auto func = parse_func(lexer);
            func->exported = exported;
            module->funcs.push_back(func);
        }
    }

    return module;
}

ref<ast::Func> Parser::parse_func(Lexer& lexer) {
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
        param.type = parse_type(lexer);
        func->params.push_back(param);
        token = lexer.peek();
        if (token.kind == TokenComma) {
            lexer.next();
            token = lexer.peek();
        }
    }
    lexer.expect(TokenRightParen);
    func->return_type = parse_type(lexer);
    func->root = parse_block(lexer);
    
    return func;
}

ref<ast::Stmt> Parser::parse_stmt(Lexer& lexer) {
    if (lexer.test("if")) {
        return parse_if(lexer);        
    } else if (lexer.test("while")) {
        return parse_if(lexer);        
    } else if (lexer.test("for")) {
        return parse_if(lexer);        
    } else if (lexer.test("return")) {
        return parse_return(lexer);
    } else if (lexer.test("let")) {
        return parse_return(lexer);
    } else if (lexer.test("const")) {
        return parse_return(lexer);
    } else if (lexer.test(TokenIdentifier)) {
        return parse_assign(lexer);
    } else if (lexer.test(TokenLeftCurly)) {
        return parse_block(lexer);
    } else {
        parser_error(lexer.peek(), "Expected statement\n");
    }
}

ref<ast::Stmt> Parser::parse_if(Lexer& lexer) {
    lexer.expect(TokenIdentifier);
    auto stmt = make_ref<ast::If>();
    auto expr = parse_expr(lexer);
    stmt = make_ref<ast::If>();
    stmt->condition = expr;
    stmt->then_stmt = parse_block(lexer);
    if (lexer.test("else")) {
        lexer.next();
        if (lexer.test("if")) {
            stmt->else_stmt = parse_if(lexer);
        } else {
            stmt->else_stmt = parse_block(lexer);
        }
    }
    return stmt;
}

ref<ast::Stmt> Parser::parse_for(Lexer& lexer) {
    auto stmt = make_ref<ast::For>();
    lexer.expect(TokenIdentifier);
    stmt->name = lexer.token_to_string(lexer.expect(TokenIdentifier));
    if(!lexer.test("in")) {
        parser_error(lexer.next(), "Expected \'in\' in for statement");
    }
    lexer.expect(TokenIdentifier);
    stmt->iterator = parse_expr(lexer);
    stmt->loop = parse_block(lexer);
    return stmt;
}

ref<ast::Stmt> Parser::parse_while(Lexer& lexer) {
    auto stmt = make_ref<ast::While>();
    lexer.expect(TokenIdentifier);
    stmt->condition = parse_expr(lexer);
    stmt->loop = parse_block(lexer);
    return stmt;
}

ref<ast::Stmt> Parser::parse_return(Lexer& lexer) {
    lexer.expect(TokenIdentifier);
    auto stmt = make_ref<ast::Return>();
    if (!lexer.test(TokenSemiColon)) {
        stmt->value = parse_expr(lexer);
    }
    lexer.expect(TokenSemiColon);
    return stmt;
}

ref<ast::Stmt> Parser::parse_var(Lexer& lexer) {
    auto stmt = make_ref<ast::VarDecl>();
    if(lexer.test("const")) {
        stmt->is_const = true;
    } else {
        stmt->is_const = false;
    }
    lexer.expect(TokenIdentifier);
    stmt->name = lexer.token_to_string(lexer.expect(TokenIdentifier));
    if (lexer.test(TokenColon)) {
        lexer.next();
        stmt->type = parse_type(lexer);
    } else {
        stmt->type.is_unknown = true;
    }
    lexer.expect(TokenEquals);
    stmt->value = parse_expr(lexer);
    lexer.expect(TokenSemiColon);
    return stmt;
}

ref<ast::Stmt> Parser::parse_assign(Lexer& lexer) {
    auto stmt = make_ref<ast::Assign>();
    stmt->name = lexer.token_to_string(lexer.expect(TokenIdentifier));
    lexer.expect(TokenEquals);
    stmt->value = parse_expr(lexer);
    lexer.expect(TokenSemiColon);
    return stmt;
}

ref<ast::Stmt> Parser::parse_block(Lexer& lexer) {
    auto stmt = make_ref<ast::Block>();
    lexer.expect(TokenLeftCurly);
    while(!lexer.test(TokenRightCurly)) {
        stmt->stmts.push_back(parse_stmt(lexer));
    }
    lexer.expect(TokenRightCurly);
    return stmt;
}

ref<ast::Expr> Parser::parse_expr(Lexer& lexer) {
    return parse_bin_expr(lexer, 0);
}

ref<ast::Expr> Parser::parse_primary_expr(Lexer& lexer) {
    if (lexer.test(TokenIdentifier)) {
        return parse_ident(lexer);
    } else if (lexer.test(TokenNumber)) {
        return parse_number(lexer);
    } else if (lexer.test(TokenString)) {
        return parse_string(lexer);
    } else {
        printf("Found token: %s\n", get_token_name(lexer.peek().kind));
        parser_error(lexer.peek(), "Expected expression");
    }
}

ref<ast::Expr> Parser::parse_ident(Lexer& lexer) {
    auto token = lexer.expect(TokenIdentifier);
    auto expr = make_ref<ast::Identifier>();
    expr->name = lexer.token_to_string(token);
    return expr;
}

ref<ast::Expr> Parser::parse_number(Lexer& lexer) {
    auto token = lexer.expect(TokenNumber);
    if (lexer.is_token_int_or_float(token)) {
        auto expr = make_ref<ast::Float>();
        expr->value = lexer.token_to_float(token);
        return expr;
    } else {
        auto expr = make_ref<ast::Integer>();
        expr->value = lexer.token_to_int(token);
        return expr;
    }
}

ref<ast::Expr> Parser::parse_string(Lexer& lexer) {
    auto token = lexer.expect(TokenString);
    auto expr = make_ref<ast::String>();
    expr->value = lexer.token_to_string(token);
    return expr;
}

u8 Parser::parse_prec(Token token) {
    switch(token.kind) {
        case TokenPlus:
        case TokenMinus:
            return 1;
        case TokenAstericks:
        case TokenForwardSlash:
            return 2;
        default:
            return 0;
    }
}

ast::BinaryExpr::Kind Parser::parse_bin_op_kind(Token token) {
    switch(token.kind) {
        case TokenPlus:
            return ast::BinaryExpr::KindAdd;
        case TokenMinus:
            return ast::BinaryExpr::KindSubtract;
        case TokenAstericks:
            return ast::BinaryExpr::KindMultiply;
        case TokenForwardSlash:
            return ast::BinaryExpr::KindDivide;
        case TokenEquals:
            return ast::BinaryExpr::KindEqual;
        case TokenExclamationEquals:
            return ast::BinaryExpr::KindNotEqual;
        case TokenLessThen:
            return ast::BinaryExpr::KindLessThan;
        case TokenGreaterThen:
            return ast::BinaryExpr::KindGreaterThan;
        case TokenLessThenEquals:
            return ast::BinaryExpr::KindLessThanEqual;
        case TokenGreaterThenEquals:
            return ast::BinaryExpr::KindGreaterThanEqual;
        default:
            parser_error(token, "Unknown binary operator");
    }
}

ref<ast::Expr> Parser::parse_bin_expr(Lexer& lexer, u8 prec) {
    auto lhs = parse_primary_expr(lexer);
    while (true) {
        auto token = lexer.peek();
        u8 new_prec = parse_prec(token);
        if (new_prec <= prec) {
            break;
        }
        lexer.next();
        auto rhs = parse_bin_expr(lexer, new_prec);
        auto expr = make_ref<ast::BinaryExpr>();
        expr->bin_kind = parse_bin_op_kind(token);
        expr->lhs = lhs;
        expr->rhs = rhs;
        lhs = expr;
    }
    return lhs;
}

ast::Type Parser::parse_type(Lexer& lexer) {
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