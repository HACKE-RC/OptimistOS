#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP

#include <cstddef>
#include <cstdint>

#include "../idt/isr.hpp"
#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"
#include "../lib/lock.hpp"
#include "../smp/smp.hpp"

#define PROCESS_MAX_THREADS 64
extern uint64_t processCount;

struct PageTable;
struct thread;
struct process;

typedef enum {
    PRIORITY_IDLE,
    PRIORITY_NORMAL,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} threadPriority;

struct thread{
    uint64_t threadID;
    registers regs;
    threadPriority priority;
    uint64_t stackAddress;
    uint64_t cpuID;
};

struct process{
    uint64_t processID;
    PageTable *PML4;
    uint64_t state;
    struct thread threads[PROCESS_MAX_THREADS];
    uint32_t threadCount;
    process* next;
};


extern process* processHead;
extern process* createThreadAsProcess(void (*entryPoint), threadPriority priority, uint64_t cpuID);
extern process* getProcess(uint64_t processID);

#endif
