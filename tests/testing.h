#pragma once
#include <stdio.h>

#define TEST_ASSERT(cond) if(cond) { \
    printf("ASSERT FAILED %s in %s: %d", #cond, __FUNCTION__, __LINE__); \
    return 1; \
} \
