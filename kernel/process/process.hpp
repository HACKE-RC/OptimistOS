#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP
#include <cstddef>
#include <cstdint>
#include "../idt/isr.hpp"
struct PageTable;
#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"

typedef struct{
    uint64_t processID;
    registers regs;
    PageTable *PML4;
//    uint8_t priority;
//    uint64_t state;
    uint64_t cpuID;
    uint64_t stackAddress;
} process;

#endif
