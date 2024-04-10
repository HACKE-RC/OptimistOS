#ifndef MLFQ_SCHEDULER_HPP
#define MLFQ_SCHEDULER_HPP
#include "../lib/lock.hpp"
#include "../process/process.hpp"
extern thread* runningThread;
extern threadList* prioritySort(threadList* tList);
extern uint32_t getProcessorCount();
extern void runThread(cpuRegs* regs);
extern void execute(thread* threadInfo, int processorNo, uint8_t quanta);
extern int getQuanta(threadPriority threadPriority);
extern threadPriority changePriority(threadPriority currentPriority);
#endif