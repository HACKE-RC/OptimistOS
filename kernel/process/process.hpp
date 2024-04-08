#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP

#include <cstddef>
#include <cstdint>

#include "../idt/isr.hpp"
#include "../idt/idt.hpp"
#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"
#include "../lib/lock.hpp"
#include "../smp/smp.hpp"
#include "../memory/malloc.hpp"
#include "../lib/string.hpp"

#define PROCESS_MAX_THREADS 64
#define THREAD_STACK_SIZE 20480
#define THREAD_STACK_ADDR 0x70000000000

extern uint64_t processCount;
struct PageTable;
struct thread;
struct process;

#define PIT_COUNTER0 0x40
#define PIT_CMD 0x43
#define PIT_BINARY 0x00
#define PIT_MODE3 0x04
#define PIT_RW_BOTH 0x30

static const uint64_t baseFrequency = 1193182;
extern uint64_t frequency;
// static const uint16_t divisor = 65535;
static uint64_t timeSinceBoot = 0;


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

struct thread{
    uint64_t threadID;
    cpuRegs regs;
    threadPriority priority;
    uintptr_t stackAddress;
    uint64_t cpuID;
    uintptr_t kernelStack;
    uintptr_t entryPoint;
};

struct threadList{
    thread* threadInfo;
    threadList* next;
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

void setupThreadContext(thread* thread, uintptr_t entryPoint, bool user, threadState state);
inline PageTable* getPageMap(bool user);
extern processInternal* setupProcessInfo();
extern processInternal* initProcesses();
extern process* getProcessInfo(uint64_t processID);
bool removeProcessFromList(processInternal* process);
void registerProcess(processInternal* process);
thread* createThreadInternal(void (*entrypoint)(), threadPriority priority, uint64_t cpuID, threadState state, bool user);
extern process* createProcessFromRoutine(void (*entryPoint)(), threadPriority priority, uint64_t cpuID, threadState state, bool user);
process* processInternalToProcess(processInternal* processIn, process* processOut);
extern int getPid(process* process);
extern void pitInit(uint8_t hertz);
extern uint64_t getPITCount();
extern void sleep(int seconds);
static void addThreadToList(thread* thread);
extern threadList* threadHead;
extern uint32_t processMutex;
extern uint32_t threadMutex;
#endif
