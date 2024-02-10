#ifndef PAGING_CPP_SMP_HPP
#define PAGING_CPP_SMP_HPP

#include <mutex>
#include "../boot.h"
#include "../process/process.hpp"
#include "../kernelEntry.hpp"

typedef struct{
   uint32_t lock;
   uint32_t lapicID;
   process *currentProcess;
   process *processList;
   uint64_t processSize;
   uint64_t processIndex;
   uint64_t processPRCount;
   uint64_t lastIdleTime;
   uint64_t totalTime;
} __attribute__((packed)) cpuInfo;


extern void initSMP();
// bootStrapProcessor LAPIC ID
extern int bspLAPICID;
extern int cpuCount;
extern int cpusStarted;
extern cpuInfo* cpuInformation[];
void initOtherCPUs(limine_smp_info *smpInfo);
#endif
