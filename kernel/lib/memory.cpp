#include "memory.hpp"

void mmioWrite(void* address, uint32_t value){
    *((volatile uint32_t*)address) = value;
}

void mmioWrite(void* address, uint64_t value){
    *((volatile uint64_t*)address) = value;
}

extern uint32_t mmioRead32(void* address){
    return *((uint32_t*)address);
}

extern uint64_t mmioRead64(void* address){
    return *((uint64_t*)address);
}
