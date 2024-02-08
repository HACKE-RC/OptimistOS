#ifndef PAGING_CPP_SMP_HPP
#define PAGING_CPP_SMP_HPP
#include "../boot.h"
#include "../process/process.hpp"

typedef struct{
   bool lock;
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
extern cpuInfo* cpuInformation[];
void printHello(limine_smp_info *pInfo);
#endif
