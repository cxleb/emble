#include "lexer.h"
#include "error.h"

void lexer_error [[noreturn]] (Token token, const char* message, ...) {
    fprintf(stderr, "%llu:%llu: ", token.line, token.col);
    va_list args;
    va_start(args, message);
    verror(message, args);
    va_end(args);     
}

const char* TokenKindNames[] = {
#define A(name) #name,
    TOKENS(A)
#undef A
};
    
const char* get_token_name(TokenKind kind) {
    return TokenKindNames[kind];
}

Lexer::Lexer(std::vector<char>&& source) {
    this->source = std::move(source);
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
                break;
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
    eat_whitespace();
    
    Token token;
    token.offset = at;
    token.line = line;
    token.col = col;
    token.size = 0;
    token.kind = TokenEndOfFile;
    
    if (at >= source.size()) {
        return token;
    }
    
    switch(source[at]) {
        case '^':
            token.kind = TokenCaret;
            goto single_char;
        case '&':
            token.kind = TokenAmpersand;
            goto single_char;
        case '*':
            token.kind = TokenAstericks;
            goto single_char;
        case '+':
            {
                if (at + 1 < source.size() && source[at + 1] == '+') {
                    token.kind = TokenPlusPlus;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                }
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenPlusEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenPlus;
                    goto single_char;
                }
            }
        case '-':
            {
                if (at + 1 < source.size() && source[at + 1] == '-') {
                    token.kind = TokenMinusMinus;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                }
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenMinusEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenMinus;
                    goto single_char;
                }
            }
        case '=':
            {
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenEqualsEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenEquals;
                    goto single_char;
                }
            }
        case ':':
            token.kind = TokenColon;
            goto single_char;
        case ';':
            token.kind = TokenSemiColon;
            goto single_char;
        case '.':
            token.kind = TokenDot;
            goto single_char;
        case ',':
            token.kind = TokenComma;
            goto single_char;
        case '/':
            token.kind = TokenForwardSlash;
            goto single_char;
        case '(':
            token.kind = TokenLeftParen;
            goto single_char;
        case ')':
            token.kind = TokenRightParen;
            goto single_char;
        case '[':
            token.kind = TokenLeftBracket;
            goto single_char;
        case ']':
            token.kind = TokenRightBracket;
            goto single_char;
        case '{':
            token.kind = TokenLeftCurly;
            goto single_char;
        case '}':
            token.kind = TokenRightCurly;
            goto single_char;
        case '<':
            {
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenLessThenEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenLessThen;
                    goto single_char;
                }
            }
        case '>':
            {
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenGreaterThenEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenGreaterThen;
                    goto single_char;
                }
            }
        case '!':
            {
                if (at + 1 < source.size() && source[at + 1] == '=') {
                    token.kind = TokenExclamationEquals;
                    at++;
                    col++;
                    token.size = 2;
                    goto end;
                } else {
                    token.kind = TokenExclamation;
                    goto single_char;
                }
            }
        default:
            break;
    }

    if (isalpha(source[at]) || source[at] == '_') {
        token.kind = TokenIdentifier;
        while (at < source.size() && (isalnum(source[at]) || source[at] == '_')) {
            at++;
            col++;
        }
        token.size = at - token.offset;
        goto end;
    } else if (isdigit(source[at]) || source[at] == '.') {
        token.kind = TokenNumber;
        while (at < source.size() && (isdigit(source[at]) || source[at] == '.')) {
            at++;
            col++;
        }
        token.size = at - token.offset;
        goto end;
    } else if (source[at] == '"') {
        token.kind = TokenString;
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
        lexer_error(token, "Unknown character: %c", source[at]);
    }

    goto end;
single_char:
    at++;
    col++;
    token.size = 1;
end:
    return token;
}

void Lexer::copy_token(char* buf, uint32_t size, Token token) {
    if (token.size > size) {
        lexer_error(token, "copy_token: buffer too small(%d)", size);
    }
    for (uint64_t i = 0; i < token.size; i++) {
        buf[i] = source[token.offset + i];
    }
    buf[token.size] = '\0';
}

bool Lexer::is_token_int_or_float(Token token) {
    if (token.kind == TokenNumber) {
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

bool Lexer::test(TokenKind kind) {
    auto token = peek();
    return token.kind == kind;
}


bool Lexer::test(const std::string& str) {
    auto token = peek();
    if (token.kind != TokenIdentifier) {
        return false;
    }
    char buf[64];
    copy_token(buf, 64, token);
    return strcmp(buf, str.c_str()) == 0;
}

Token Lexer::expect(TokenKind kind) {
    auto token = next();
    if (token.kind != kind) {
        lexer_error(token, "Expected token %s, got %s\n", 
            get_token_name(kind), 
            get_token_name(token.kind));
    }
    return token;
}