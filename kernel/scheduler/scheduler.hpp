#ifndef MLFQ_SCHEDULER_HPP
#define MLFQ_SCHEDULER_HPP
#include "../lib/lock.hpp"
#include "../process/process.hpp"

threadList* prioritySort(threadList* tList);
int getProcessorCount();
void runThreads();
void execute(thread* threadInfo, int processorNo, uint8_t quanta);
int getQuanta(threadPriority threadPriority);
threadPriority changePriority(threadPriority currentPriority);
extern "C" void saveContext(cpuRegs* context);
extern "C" void loadContext(cpuRegs* context);
#endif