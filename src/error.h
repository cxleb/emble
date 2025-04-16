#pragma once

#include "lexer.h"

#ifdef NDEBUG
#define assert(cond) 
#else
#define assert(cond) \
    if (!(cond)) { \
        error("assert failed %s in %s at %d\n", #cond, __FUNCTION__, __LINE__); \
    } 
#endif

void error [[noreturn]] (const char* message, ...);
void parser_error [[noreturn]] (Token token, const char* message, ...);