#include "process.hpp"
static uint32_t processMutex = 0;

process* processHead;
uint64_t processCount = 0;

process* createThreadAsProcess(void (*entryPoint), threadPriority priority, uint64_t cpuID){
    lock(processMutex);
    process* processInfo;

    cpuInfo* cpu = getCPU(cpuID);
    if (cpu == nullptr){
        haltAndCatchFire(__FILE__, __LINE__, "Invalid CPU Requested!!");
        return nullptr;
    }

    processInfo->next = nullptr;
    processInfo->processID = 7 * (processCount);
    processInfo->threadCount = 0;
    processInfo->state = 0;

    thread currentThread = {
        .threadID = processInfo->threadCount,
//        .regs = ,
        .priority = PRIORITY_HIGH,
//        .stackAddress =
        .cpuID = cpu->cpuNumber,
    };

    processInfo->threads[currentThread.threadID] = currentThread;


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
