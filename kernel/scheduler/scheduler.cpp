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
    lock(threadMutex);
//    asm volatile("cli");

    if (processHead == nullptr){
        unlock(threadMutex);
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

//    (3, (uint64_t)PML4);
    writeCrReg(3, (uint64_t)runningThread->parentProcess->PML4);
    writeCrReg(3, (uint64_t)runningThread->parentProcess->PML4);

//    if (runningThread->startTime == 0){
//        runningThread->startTime = getPITCount();
//        runningThread->quanta = pitTicks + getQuanta(runningThread->priority);
//    }

    runningThread->state = THREAD_RUNNING;
    contextSwitch(&runningThread->regs);
    auto (*function)() = reinterpret_cast<void(*)()>(runningThread->entryPoint);
    function();

    threadCount--;
    runningThread->state = THREAD_SUSPENDED;

//    if (getPITCount() >= runningThread->quanta){
//        runningThread->startTime = 0;
//        runningThread->quanta = 0;
//        removeThreadFromList(runningThread);
//    }

    unlock(threadMutex);
}


uint32_t getProcessorCount(){
    return cpusStarted;
}

void execute(thread* threadInfo, int processorNo, uint8_t quanta){
    cpuInfo* cpu = getCPUInfo(processorNo);
    smpInfo* cpuInfo = getSMPInfo(processorNo);
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