#include "lexer.h"


Lexer::Lexer(std::vector<char> source) {
    this->source = source;
    at = 0;
    line = 0; 
    col = 0;
}

void Lexer::eat_whitespace() {
    while (at < source.size()) {
        auto c = source[at];
        if (c == ' ' || c == '\t') {
            at++;
            col++;
        } else if (c == '\n') {
            at++;
            line++;
            col = 0;
        } else if (c == '/') {
            at++;
            if (at < source.size() && source[at] == '/') {
                at++;
                while (at < source.size() && source[at] != '\n') {
                    at++;
                }
                col = 0;
                line++;
            } else {
                at--;
            }
        } 
        else {
            break;
        }
    }
}

Token Lexer::peek() {
    // save lexer state
    auto saved_at = at;
    auto saved_line = line;
    auto saved_col = col;

    auto token = next();
    
    // restore lexer state
    at = saved_at;
    line = saved_line;
    col = saved_col;
    
    return token;
}

Token Lexer::next() {
    Token token;
    token.offset = at;
    token.line = line;
    token.col = col;
    token.size = 0;
    token.type = TokenEndOfFile;
    
    eat_whitespace();

    if (at >= source.size()) {
        return token;
    }
    
    switch(source[at]) {
        case '^':
            token.type = TokenCaret;
            goto single_char;
        case '&':
            token.type = TokenAmpersand;
            goto single_char;
        case '*':
            token.type = TokenAstericks;
            goto single_char;
        case '+':
            token.type = TokenPlus;
            goto single_char;
        case '-':
            token.type = TokenMinus;
            goto single_char;
        case '=':
            token.type = TokenEquals;
            goto single_char;
        case ';':
            token.type = TokenSemiColon;
            goto single_char;
        case '.':
            token.type = TokenDot;
            goto single_char;
        case ',':
            token.type = TokenComma;
            goto single_char;
        case '/':
            token.type = TokenForwardSlash;
            goto single_char;
        case '(':
            token.type = TokenLeftParen;
            goto single_char;
        case ')':
            token.type = TokenRightParen;
            goto single_char;
        case '[':
            token.type = TokenLeftBracket;
            goto single_char;
        case ']':
            token.type = TokenRightBracket;
            goto single_char;
        case '{':
            token.type = TokenLeftCurly;
            goto single_char;
        case '}':
            token.type = TokenRightCurly;
            goto single_char;
        case '<':
            token.type = TokenLessThen;
            goto single_char;
        case '>':
            token.type = TokenGreaterThen;
            goto single_char;
        default:
            break;
    }

    if (isalpha(source[at]) || source[at] == '_') {
        token.type = TokenIdentifier;
        while (at < source.size() && (isalnum(source[at]) || source[at] == '_')) {
            at++;
            col++;
        }
        token.size = at - token.offset;
        goto end;
    } else if (isdigit(source[at]) || source[at] == '.') {
        token.type = TokenNumber;
        while (at < source.size() && (isdigit(source[at]) || source[at] == '.')) {
            at++;
            col++;
        }
        token.size = at - token.offset;
        goto end;
    } else if (source[at] == '"') {
        token.type = TokenString;
        at++;
        col++;
        while (at < source.size() && source[at] != '"') {
            at++;
            col++;
        }
        if (at < source.size()) {
            at++;
            col++;
        }
        token.size = at - token.offset;
        goto end;
    } else {
        // Unknown character
        printf("%d:%d Unknown character: %c\n", line, col, source[at]);
        exit(1);
    }

    goto end;
single_char:
    at++;
    col++;
end:
    return token;
}

void Lexer::copy_token(char* buf, uint32_t size, Token token) {
    if (token.size > size) {
        printf("%d:%d: copy_token: could not copy as buffer was too small\n", token.line, token.col);
        exit(1);
    }
    for (uint64_t i = 0; i < token.size; i++) {
        buf[i] = source[token.offset + i];
    }
    buf[token.size] = '\0';
}

bool Lexer::is_token_int_or_float(Token token) {
    if (token.type == TokenNumber) {
        auto start = token.offset;
        auto end = token.offset + token.size;
        bool has_dot = false;
        for (uint64_t i = start; i < end; i++) {
            if (source[i] == '.') {
                return true;
            }
        }
    }
    return false;
}

double Lexer::token_to_float(Token token) {
    char buf[64];
    copy_token(buf, 64, token);
    return atof(buf);
}

uint64_t Lexer::token_to_int(Token token) {
    char buf[64];
    copy_token(buf, 64, token);
    return atoi(buf);
}

std::string Lexer::token_to_string(Token token) {
    char buf[64];
    copy_token(buf, 64, token);
    return std::string(buf, token.size);
}