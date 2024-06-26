#include "scheduler.hpp"
uint32_t threadMutex = 0;
thread* runningThread = nullptr;
extern "C" void contextSwitch(cpuRegs* regs);

threadList* prioritySort(threadList* tList){
    if (tList == nullptr || tList->next == nullptr){
        return tList;
    }

    bool swapped;
    threadList* currentThread;

    do {
        swapped = false;
        currentThread = tList;

        while (currentThread->next->threadInfo != nullptr) {
            if (currentThread->threadInfo->priority < currentThread->next->threadInfo->priority) {
                thread *temp = currentThread->threadInfo;
                currentThread->threadInfo = currentThread->next->threadInfo;
                currentThread->next->threadInfo = temp;
                swapped = true;
            }
            currentThread = currentThread->next;
        }
    }
    while (swapped);

    return tList;
}

void runThread(cpuRegs* regs){
    asm volatile("sti");
    if (processHead == nullptr || threadHead == nullptr || threadHead->threadInfo == nullptr || threadCount == 0){
        return;
    }

    threadList* tList = threadHead;

    if (runningThread == nullptr ){
       runningThread = tList->threadInfo;
    }
    else{
        runningThread->regs = *regs;
        runningThread->state = THREAD_READY;
        runningThread = getNextThread(runningThread);

        if (runningThread == nullptr){
            runThread(regs);
        }
    }

    lock(runningThread->lock);

    writeCrReg(3, (uint64_t)runningThread->parentProcess->PML4);

    runningThread->state = THREAD_RUNNING;
//    runningThread->regs.rip = runningThread->entryPoint;
    runningThread->regs.rdi = (uint64_t)runningThread->entryPoint;
    runningThread->regs.rip = (uint64_t)schedulerWrapper;

    *regs = runningThread->regs;

    contextSwitch(&runningThread->regs);

    writeCrReg(3, (uint64_t)kernelPML4);
    runningThread->state = THREAD_SUSPENDED;
    unlock(runningThread->lock);
    threadCount--;
}

void schedulerWrapper(uint64_t func){
    ((uint64_t(*)())func)();
    runningThread->state = THREAD_SUSPENDED;
    unlock(runningThread->lock);
    unlock(lockx);
    threadCount--;
    asm volatile("sti");
    thread* next = getNextThread(runningThread);
    runThread(&next->regs);
    for (;;){
    }
}


uint32_t getProcessorCount(){
    return cpusStarted;
}


int getQuanta(threadPriority threadPriority){
    switch (threadPriority) {
        case PRIORITY_HIGH:
            return 8;
        case PRIORITY_MEDIUM:
            return 6;
        case PRIORITY_LOW:
            return 3;
        default:
            return 3;
    }
}

threadPriority changePriority(threadPriority currentPriority){
    switch (currentPriority) {
        case PRIORITY_HIGH:
            return PRIORITY_LOW;
        case PRIORITY_LOW:
            return PRIORITY_MEDIUM;
        case PRIORITY_MEDIUM:
            return PRIORITY_HIGH;
    }
    return PRIORITY_LOW;
}