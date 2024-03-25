#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP

#include <cstddef>
#include <cstdint>
#include "../idt/isr.hpp"
#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"
#include "../lib/lock.hpp"
#include "../smp/smp.hpp"

struct PageTable;

struct process{
    uint64_t processID;
    registers regs;
    PageTable *PML4;
    uint8_t priority;
    uint64_t state;
    uint64_t cpuID;
    uint64_t stackAddress;
};

typedef enum {
    PROCESS_PRIORITY_IDLE,
    PROCESS_PRIORITY_NORMAL,
    PROCESS_PRIORITY_MEDIUM,
    PROCESS_PRIORITY_HIGH
} processPriority;

extern process* createThreadAsProcess(void (*entryPoint), processPriority priority, uint64_t cpuID);

#endif
