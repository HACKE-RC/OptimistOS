#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP

#include <cstddef>
#include <cstdint>

#include "../idt/isr.hpp"
#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"
#include "../lib/lock.hpp"
#include "../smp/smp.hpp"
#include "../memory/malloc.hpp"

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

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_SUSPENDED
} threadState;

struct thread{
    uint64_t threadID;
    registers regs;
    threadPriority priority;
    uintptr_t stackAddress;
    uint64_t cpuID;
    void (*entryPoint)();
};

struct processInternal{
    uint64_t processID;
    PageTable *PML4;
    struct thread threads[PROCESS_MAX_THREADS];
    uint32_t threadCount;
    processInternal* next;
    bool userProcess;
};

struct process{
    uint64_t processID;
    PageTable *PML4;
    struct thread threads[PROCESS_MAX_THREADS];
    uint32_t threadCount;
};

processInternal* processHead;
inline PageTable* getPageMap(bool user);
extern process* createEmptyProcess();
extern processInternal* initProcesses();
extern process* getProcess(uint64_t processID);
extern thread* createThread(void (*entrypoint), threadPriority priority, uint64_t cpuID, uint64_t threadState);
extern process* createProcessFromRoutine(void (*entryPoint), threadPriority priority, uint64_t cpuID, threadState state, bool user);
#endif
