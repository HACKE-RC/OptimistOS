#include "scheduler.hpp"
uint32_t threadMutex = 0;

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

void runThreads(){
    lock(threadMutex);
    int processorCount = getProcessorCount();
    thread** execThreadList = (thread**)mallocx(processorCount * sizeof(thread*));
    int threadsScheduled = 0;

    threadList* sortedList = prioritySort(threadHead);
    threadList* sortedList2 = sortedList;
    threadList* sortedList3 = sortedList;

    while(sortedList->threadInfo != nullptr && sortedList->next != nullptr){
        for (int i = 0; i < processorCount && sortedList->threadInfo != nullptr; i++){
            execThreadList[i] = sortedList->threadInfo;
            sortedList = sortedList->next;
        }

        for (int i = 0; i < processorCount; i++){
            if (execThreadList[i] != nullptr && sortedList2->threadInfo != nullptr){
                execute(execThreadList[i], i, getQuanta(execThreadList[i]->priority));
                execThreadList[i]->priority = changePriority(execThreadList[i]->priority);
                sortedList2->threadInfo->priority = execThreadList[i]->priority;
                sortedList2 = sortedList2->next;
                threadsScheduled++;
            }
        }
    }

    freex(execThreadList);
    threadHead = sortedList3;
    asm volatile("hlt");
    unlock(threadMutex);
}

int getProcessorCount(){
    return cpusStarted;
}

void execute(thread* threadInfo, int processorNo, uint8_t quanta){
    cpuInfo* cpu = getCPUInfo(processorNo);
    smpInfo* cpuInfo = getSMPInfo(processorNo);
    saveContext(&threadInfo->regs);
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