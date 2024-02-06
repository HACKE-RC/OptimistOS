#include "string.hpp"

void memoryset(void *start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
    {
        *(uint8_t *)((uint64_t)start + i) = value;
    }
}

bool memorycmp(const void *s1, const void *s2, size_t n){
    const uint8_t *p1 = (const uint8_t*)s1;
    const uint8_t *p2 = (const uint8_t*)s1;

    for (size_t i = 0; i < n; i++){
        if (p1[i] != p2[i]){
            return false;
        }
    }
    return true;
}
