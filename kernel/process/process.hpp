#ifndef PAGING_CPP_PROCESS_HPP
#define PAGING_CPP_PROCESS_HPP

#include <cstddef>
#include <cstdint>

#include "../paging/paging.hpp"
#include "../paging/pageFrameAllocator.hpp"
#include "../lib/lock.hpp"
#include "../memory/malloc.hpp"
#include "../lib/string.hpp"
#include "../smp/smp.hpp"
#include "../scheduler/schedulerDecls.hpp"

#define THREAD_STACK_SIZE 20480
#define THREAD_STACK_ADDR 0x70000000000

extern uint64_t processCount;
extern uint64_t threadCount;
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


extern uint32_t threadMutex;
extern uint32_t processMutex;
extern threadList* threadHead;
extern uint64_t getPITCount();
extern void sleep(int seconds);
extern int getPid(process* process);
extern void pitInit(uint8_t hertz);
inline PageTable* getPageMap(bool user);
extern processInternal* initProcesses();
extern void sleepMS(int milliseconds);
extern processInternal* setupProcessInfo();
static void addThreadToList(thread* thread);
void registerProcess(processInternal* process);
extern thread* getThreadInfo(uint64_t threadID);
extern process* getProcessInfo(uint64_t processID);
bool removeProcessFromList(processInternal* process);
extern thread* getNextThread(thread* currentThread);
extern threadList* getThreadList(uint64_t threadID, bool next);
process* processInternalToProcess(processInternal* processIn, process* processOut);
void setupThreadContext(thread* thread, uintptr_t entryPoint, bool user, threadState state);
thread* createThreadInternal(uintptr_t entrypoint, threadPriority priority, uint64_t cpuID, threadState state, bool user);
extern process* createProcessFromRoutine(uintptr_t entryPoint, threadPriority priority, uint64_t cpuID, threadState state, bool user);
extern bool removeThreadFromList(thread* threadToRemove);
#endif
