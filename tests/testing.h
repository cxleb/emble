#pragma once
#include <stdio.h>
#include "utils.h"

#define TEST_ASSERT(cond) if(cond) { \
    printf("ASSERT FAILED %s in %s: %d", #cond, __FUNCTION__, __LINE__); \
    return 1; \
} \

std::vector<char> to_source(const std::string& source) {
    return std::vector<char>(source.begin(), source.end());
}