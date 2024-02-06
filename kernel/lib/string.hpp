#ifndef PAGING_CPP_STRING_HPP
#define PAGING_CPP_STRING_HPP
#include <cstdint>
#include <cstddef>

extern void memoryset(void *start, uint8_t value, uint64_t num);
bool memorycmp(const void *s1, const void *s2, size_t n);
#endif //PAGING_CPP_STRING_HPP
