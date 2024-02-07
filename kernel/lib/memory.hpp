#ifndef PAGING_CPP_MEMORY_HPP
#define PAGING_CPP_MEMORY_HPP

#include <cstdint>
#include <cstddef>

extern void mmioWrite(void* address, uint32_t value);
extern void mmioWrite(void* address, uint64_t value);

extern uint32_t mmioRead32(void* address);
extern uint64_t mmioRead64(void* address);


#endif
