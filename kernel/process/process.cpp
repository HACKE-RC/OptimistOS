#include "process.hpp"
static uint32_t processMutex = 0;

process* createThreadAsProcess(void (*entryPoint), processPriority priority, uint64_t cpuID){
    lock(processMutex);

    cpuInfo* cpu = getCPU(cpuID);
    if (cpu == nullptr){
        haltAndCatchFire(__FILE__, __LINE__, "Invalid CPU Requested!!");
        return nullptr;
    }

    unlock(processMutex);
    return nullptr;
}
