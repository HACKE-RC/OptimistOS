#include "process.hpp"
static uint32_t processMutex = 0;

process* processHead;
uint64_t processCount = 0;
processLinkedList* processListHead = nullptr;

processLinkedList* initProcesses(){
    processLinkedList* head = (processLinkedList *)(mallocx(sizeof(processLinkedList)));
    process* processInfo;
    thread* threadInfo;
//  32 kb stack
    char* stack = (char*)(toVirtualAddr((void*)allocateFrame(32768)));

    processInfo->processID = 0;
    processInfo->PML4 = PML4;
    processInfo->threadCount = 1;
    threadInfo->threadID = 1;
    threadInfo->cpuID = 0;
    threadInfo->priority = PRIORITY_HIGH;
    threadInfo->entryPoint = idle;
    threadInfo->stackAddress = (uintptr_t)stack;
    threadInfo->regs = {
            .rsp = (uint64_t)((uintptr_t)stack + 32768)
    };
    processInfo->threads[0] = *threadInfo;

    head->processInfo = processInfo;
    head->next = nullptr;
    return head;
}

process* createEmptyProcess(){
    process* processInfo;

    if (processListHead == nullptr){
        processListHead = initProcesses();
    }

    processInfo->processID = 7 * (processCount);
    processInfo->threadCount = 1;
    processInfo->PML4 = newPML4();

    return processInfo;
}



thread* createThread(void (*entrypoint), threadPriority priority, uint64_t cpuID, uint64_t threadState){
//    char* threadStack = (char*)malloc();
    if (processListHead == nullptr){
        processListHead = initProcesses();
    }

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
    if (processListHead == nullptr){
        processListHead = initProcesses();
    }

    lock(processMutex);
    process* processInfo = createEmptyProcess();
    thread* threadInfo = createThread(entryPoint, priority, cpuID, state);

//    if (processInfo == nullptr){
//        return nullptr;
//    }
//    else if (threadInfo == nullptr){
//        return nullptr;
//    }

    processInfo->threads[threadInfo->threadID] = *threadInfo;

    unlock(processMutex);
    return processInfo;
}


extern process* getProcess(uint64_t processID){
    processLinkedList* pHead = processListHead;

    while (pHead != nullptr){
        if (pHead->processInfo->processID == processID){
            return pHead->processInfo;
        }
        pHead = pHead->next;
    }

    return nullptr;
}
