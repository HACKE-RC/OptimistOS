#ifndef PAGING_CPP_SMP_HPP
#define PAGING_CPP_SMP_HPP

//#include <mutex>
#include "../boot.h"
#include "../process/process.hpp"
#include "../kernelEntry.hpp"

struct process;

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
   uint32_t cpuNumber;
} __attribute__((packed)) cpuInfo;

typedef limine_smp_info smpInfo;
extern limine_smp_response* smpResponse;
extern limine_smp_info* getSMPInfo(uint32_t cpuNumber);
extern void initSMP();
extern uint32_t cpuCount;
extern uint32_t bspLAPICID;
extern uint32_t cpusStarted;
extern cpuInfo* cpuInformation[];
extern cpuInfo* getCPUInfo(uint32_t cpuNumber);
void initOtherCPUs(limine_smp_info *smpInfo);
#endif
