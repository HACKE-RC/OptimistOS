#include "scheduler.hpp"
uint32_t threadMutex = 0;
thread* runningThread = nullptr;
cpuRegs savedRegs;
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
    runningThread->regs.rip= runningThread->entryPoint;
    runningThread->regs.rdi = (uintptr_t)regs;
    runningThread->regs.cs = (uintptr_t)threadDone;
    savedRegs = *regs;
    *regs = runningThread->regs;
//    asm volatile("push 0x1234");

    contextSwitch(&runningThread->regs);
}

void threadDone(uintptr_t regs){
    e9_printf("\n in tdone \n ");
    writeCrReg(3, (uint64_t)kernelPML4);
    cpuRegs *registers = (cpuRegs*)regs;
    *registers= savedRegs;
    runningThread->state = THREAD_SUSPENDED;
    unlock(runningThread->lock);
    threadCount--;
    unlock(lockx);
    idtInitAgain();
    pitInit(5);
//    pitHandler(&savedRegs);

    for (;;){
        asm volatile ("hlt");
    }
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