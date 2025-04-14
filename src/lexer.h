#pragma once
#include <vector>
#include <string>
#include <stdint.h>

enum TokenType {
    TokenEndOfFile,
    TokenCaret,
    TokenAmpersand,
    TokenAstericks,
    TokenPlus,
    TokenMinus,
    TokenEquals,
    TokenSemiColon,
    TokenDot,
    TokenComma,
    TokenForwardSlash,
    TokenLeftParen,
    TokenRightParen,
    TokenLeftBracket,
    TokenRightBracket,
    TokenLeftCurly,
    TokenRightCurly,
    TokenLessThen,
    TokenGreaterThen,
    TokenIdentifier,
    TokenString,
    TokenNumber
};

struct Token {
    TokenType type;
    uint64_t offset;
    uint64_t size;
    uint64_t line;
    uint64_t col;
};

struct Lexer {
    std::vector<char> source;
    uint64_t at, line, col;

    Lexer(std::vector<char> source);
    void eat_whitespace();
    Token peek();
    Token next();
    // Returns true if the token is a float
    bool is_token_int_or_float(Token token);
    void copy_token(char* buf, uint32_t size, Token token);
    double token_to_float(Token token);
    uint64_t token_to_int(Token token);
    std::string token_to_string(Token token);
};