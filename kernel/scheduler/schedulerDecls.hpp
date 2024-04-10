#ifndef OPTIMISTOS_SCHEDULERDECLS_HPP
#define OPTIMISTOS_SCHEDULERDECLS_HPP

#define PROCESS_MAX_THREADS 64

#include <cstdint>

#include "../paging/pageFrameAllocator.hpp"
#include "../paging/paging.hpp"
#include "paging/paging.hpp"
typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t int_no;
    uint64_t err;

    uint64_t rip;
    uint64_t cs;
    uint64_t eFlags;
    uint64_t rsp;
    uint64_t ss;
} cpuRegs;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} threadPriority;

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_SUSPENDED
} threadState;
struct process;
struct PageTable;
struct thread{
    uint64_t threadID;
    cpuRegs regs;
    threadPriority priority;
    uintptr_t stackAddress;
    uint64_t cpuID;
    uintptr_t kernelStack;
    uintptr_t entryPoint;
    threadState state;
    process* parentProcess;
    uint64_t startTime;
    uint64_t quanta;
};

struct threadList{
    thread* threadInfo;
    threadList* next;
};

struct process{
    uint64_t processID;
    PageTable *PML4;
    struct thread threads[PROCESS_MAX_THREADS];
    uint32_t threadCount;
};



struct processInternal{
    uint64_t processID;
    PageTable *PML4;
    struct thread threads[PROCESS_MAX_THREADS];
    uint32_t threadCount;
    processInternal* next;
    bool userProcess;
};




#endif //OPTIMISTOS_SCHEDULERDECLS_HPP
