#include "process.hpp"
static uint32_t processMutex = 0;

process* processHead;
uint64_t processCount = 0;

process* createEmptyProcess(){
    process* processInfo;

    processInfo->next = nullptr;
    processInfo->processID = 7 * (processCount);
    processInfo->threadCount = 1;
    processInfo->state = 0;
    processInfo->PML4 = newPML4();

    return processInfo;
}

thread* createThread(void (*entrypoint), threadPriority priority, uint64_t cpuID, uint64_t threadState){
//    char* threadStack = (char*)malloc();

    cpuInfo* cpu = getCPU(cpuID);
    if (cpu == nullptr){
        haltAndCatchFire(__FILE__, __LINE__, "Invalid CPU Requested!!");
        return nullptr;
    }

    thread currentThread = {
            .threadID = 0,
            .regs = {},
//            .threadID = processInfo->threadCount,
//        .regs = ,
            .priority = PRIORITY_HIGH,
//        .stackAddress =
            .cpuID = cpu->cpuNumber,
    };

}

process* createThreadAsProcess(void (*entryPoint), threadPriority priority, uint64_t cpuID, threadState state){
    lock(processMutex);
    process* processInfo = createEmptyProcess();
    thread* threadInfo = createThread(entryPoint, priority, cpuID, state);

    processInfo->threads[threadInfo->threadID] = *threadInfo;

    unlock(processMutex);
    return processInfo;
}


extern process* getProcess(uint64_t processID){
    process* pHead = processHead;

    while (pHead != nullptr){
        if (pHead->processID == processID){
            return pHead;
        }
        pHead = pHead->next;
    }

    return nullptr;
}
