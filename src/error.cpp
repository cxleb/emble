#include "error.h"
#include <stdarg.h>

void error [[noreturn]] (const char* message, ...) {
    va_list ap;
    va_list args;
    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);
#ifdef NDEBUG
    exit(1);
#else
    abort();
#endif
}

void parser_error [[noreturn]] (Token token, const char* message, ...) {
    fprintf(stderr, "Error: %llu:%llu: ", token.line, token.col);
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);    
#ifdef NDEBUG
    exit(1);
#else
    abort();
#endif
}