#ifndef MLFQ_SCHEDULER_HPP
#define MLFQ_SCHEDULER_HPP
#include "../lib/lock.hpp"
#include "../process/process.hpp"
extern thread* runningThread;
threadList* prioritySort(threadList* tList);
int getProcessorCount();
void runThread(cpuRegs* regs);
void execute(thread* threadInfo, int processorNo, uint8_t quanta);
int getQuanta(threadPriority threadPriority);
threadPriority changePriority(threadPriority currentPriority);
#endif