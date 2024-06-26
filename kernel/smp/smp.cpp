#include "smp.hpp"
#include <mutex>
#include <memory/malloc.hpp>

uint32_t cpuCount = 1;
int cr3Value2 = 0;
uint32_t bspLAPICID = 0;
uint32_t cpusStarted = 1;
uintptr_t cr3Value = 0;
limine_smp_response* smpResponse;
cpuInfo* cpuInformation[256];

void initSMP(){
    cr3Value = (uintptr_t)PML4;
    e9_printf("cr3 value: %x\n", cr3Value);
    GlobalRenderer = &renderer;
    smpResponse = (struct limine_smp_response *)smpRequest.response;
    auto *smpInfo = (struct limine_smp_info*)smpRequest.response->cpus;

    cpuCount = smpResponse->cpu_count;
    bspLAPICID = smpResponse->bsp_lapic_id;

    e9_printf("num cpus: %d\n", cpuCount);

//    cpuInfo *cpu0Information = (cpuInfo *)toVirtualAddr((void*)allocateFrame(sizeof(cpuInfo)));
    auto *cpu0Information = (cpuInfo *) mallocx(sizeof(cpuInfo));
    cpu0Information->lock = 0;
    cpu0Information->lapicID = 0;
    cpu0Information->currentProcess = nullptr;
    cpu0Information->processIndex = cpu0Information->processSize = 0;
    cpu0Information->lastIdleTime = cpu0Information->totalTime = 0;
    cpu0Information->processPRCount = 0;
    cpu0Information->cpuNumber = 0;
    cpuInformation[0] = cpu0Information;

    e9_printf("Hello from CPU: %d\n", smpResponse->cpus[0]->lapic_id);
    for (uint64_t i = 1; i < (cpuCount); i++){
        smpResponse->cpus[i]->goto_address = initOtherCPUs;
    }

    while (cpusStarted < cpuCount){
    }

    e9_printf("SMP initialized\n");
}


void initOtherCPUs(limine_smp_info *smpInfo){
    initGDT();
//    isrInstall(renderer);
    initLAPIC();
    initPaging();

    GlobalRenderer->Print("cr3 -> ");
    GlobalRenderer->PrintInt(readCr3());
    GlobalRenderer->Print("\n");

    cr3Value2 = readCr3();

    while (cpusStarted < smpInfo->lapic_id){
        __asm__ volatile("nop");
    }

    cpuInfo *cpu = (cpuInfo*) mallocx(sizeof(cpuInfo));
    cpu->lock = 0;
    cpu->processPRCount = 0;
    cpu->totalTime = 0;
    cpu->lastIdleTime = 0;
    cpu->lapicID = smpInfo->lapic_id;
    cpu->processSize = cpu->processIndex = 0;
    cpu->processList = nullptr;
    cpu->currentProcess = nullptr;
    cpu->cpuNumber = cpusStarted;

    cpuInformation[cpu->cpuNumber] = cpu;
    cpusStarted++;
    while (true){
        ;;
    }
}

cpuInfo* getCPUInfo(uint32_t cpuNumber){
    if (cpuNumber > cpusStarted){
        return nullptr;
    }

    return cpuInformation[cpuNumber];
}

smpInfo* getSMPInfo(uint32_t cpuNumber){
    return smpResponse->cpus[cpuNumber];
}