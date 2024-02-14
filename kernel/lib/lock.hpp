#ifndef PAGING_CPP_LOCK_HPP
#define PAGING_CPP_LOCK_HPP
#include <cstddef>
#include <cstdint>

void lock(uint32_t &lock);
void unlock(uint32_t &lock);

#endif
