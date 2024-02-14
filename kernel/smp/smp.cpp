#include "smp.hpp"
#include <mutex>
int bspLAPICID = 0;
int cpuCount = 1;
int cpusStarted = 1;
uintptr_t  cr3Value = 0;
int cr3Value2 = 0;

cpuInfo* cpuInformation[256];

void initSMP(){
    std::mutex m;
    std::unique_lock <std::mutex> guard(m);
    cr3Value = (uintptr_t)PML4;
    e9_printf("cr3 value: %x\n", cr3Value);
    GlobalRenderer = &renderer;
    struct limine_smp_response *smpResponse = (struct limine_smp_response *)smpRequest.response;
    struct limine_smp_info *smpInfo = (struct limine_smp_info*)smpRequest.response->cpus;

    cpuCount = smpResponse->cpu_count;
    bspLAPICID = smpResponse->bsp_lapic_id;

    e9_printf("num cpus: %d\n", cpuCount);

    cpuInfo *cpu0Information = (cpuInfo *)toVirtualAddr((void*)allocateFrame(sizeof(cpuInfo)));
    cpu0Information->lock = 0;
    cpu0Information->lapicID = 0;
    cpu0Information->currentProcess = 0;
    cpu0Information->processIndex = cpu0Information->processSize = 0;
    cpu0Information->lastIdleTime = cpu0Information->totalTime = 0;
    cpu0Information->processPRCount = 0;
    cpuInformation[0] = cpu0Information;

    e9_printf("Hello from CPU: %d\n", smpResponse->cpus[0]->lapic_id);
    for (uint64_t i = 1; i < (cpuCount); i++){
//        e9_printf("i: %d\n", i);
        smpResponse->cpus[i]->goto_address = initOtherCPUs;
    }

    while (cpusStarted < cpuCount){
//        e9_printf("cpuStarted: %d\n", cpusStarted);
//        e9_printf("doing nothing up\n");
    }
//    e9_printf("CR3 value 1: %x\n", readCr3());
//    e9_printf("CR3 value: %x\n", cr3Value2);
    e9_printf("SMP initialized\n");
}

void initOtherCPUs(limine_smp_info *smpInfo){
    initGDT();
    isrInstall(renderer);
    initLAPIC();
    initPaging();

    GlobalRenderer->Print("cr3 -> ");
    GlobalRenderer->PrintInt(readCr3());
    GlobalRenderer->Print("\n");

    cr3Value2 = readCr3();

    while (cpusStarted < smpInfo->lapic_id){
        __asm__ volatile("nop");
    }

    cpuInfo *cpu = (cpuInfo *) toVirtualAddr((void*)allocateFrame(sizeof(cpuInfo)));
    cpu->lock = 0;
    cpu->processPRCount = 0;
    cpu->totalTime = 0;
    cpu->lastIdleTime = 0;
    cpu->lapicID = smpInfo->lapic_id;
    cpu->processSize = cpu->processIndex = 0;
    cpu->processList = nullptr;
    cpu->currentProcess = 0;

    cpuInformation[cpu->lapicID] = cpu;
    cpusStarted++;

    while (true){
        ;;
    }
}