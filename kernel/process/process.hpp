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
#define THREAD_STACK_SIZE 20480
#define THREAD_STACK_ADDR 0x70000000000

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
    uintptr_t kernelStack;
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

void setupThreadContext(thread* thread, void (*entryPoint)(), bool user, threadState state);
inline PageTable* getPageMap(bool user);
extern processInternal* setupProcessInfo();
extern processInternal* initProcesses();
extern process* getProcess(uint64_t processID);
void registerProcess(processInternal* process);
thread* createThreadInternal(void (*entrypoint)(), threadPriority priority, uint64_t cpuID, threadState state, bool user);
extern process* createProcessFromRoutine(void (*entryPoint), threadPriority priority, uint64_t cpuID, threadState state, bool user);
process* processInternalToProcess(processInternal* processIn, process* processOut);
#endif
