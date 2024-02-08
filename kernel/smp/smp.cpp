#include "smp.hpp"
int bspLAPICID = 0;
int cpuCount = 1;
int cpusStarted = 1;


cpuInfo* cpuInformation[256];

void initSMP(){
    struct limine_smp_response *smpResponse = (struct limine_smp_response *)smpRequest.response;
    struct limine_smp_info *smpInfo = (struct limine_smp_info*)smpRequest.response->cpus;

    cpuCount = smpResponse->cpu_count;
    bspLAPICID = smpResponse->bsp_lapic_id;

    e9_printf("num cpus: %d\n", cpuCount);

    cpuInfo *cpu0Information = (cpuInfo *)allocateFrame(sizeof(cpuInfo));
    cpu0Information->lock = false;
    cpu0Information->lapicID = 0;
    cpu0Information->currentProcess = 0;
    cpu0Information->processIndex = cpu0Information->processSize = 0;
    cpu0Information->lastIdleTime = cpu0Information->totalTime = 0;
    cpu0Information->processPRCount = 0;

    cpuInformation[0] = cpu0Information;

    e9_printf("Hello from CPU: %d\n", smpResponse->cpus[0]->lapic_id);
    for (uint64_t i = 1; i < (cpuCount); i++){
        e9_printf("i: %d\n", i);
        smpResponse->cpus[i]->goto_address = printHello;
        i++;
        e9_printf("i: %d\n", i);
    }

    while (cpusStarted < cpuCount){
        e9_printf("cpuStarted: %d\n", cpusStarted);
//        __asm__ volatile("hlt");
        e9_printf("doing nothing up\n");
    }

    e9_printf("SMP initialized\n");
    __asm__ volatile("hlt");
}

void printHello(limine_smp_info *pInfo){
    initGDT();
    idtInitAgain();
    initLAPIC();
//    haltAndCatchFire(__FILE__, __LINE__);

//    e9_printf("Hello from CPU: \n");
//    __asm__ volatile("hlt");
    //    if (pInfo == nullptr){
//        haltAndCatchFire(__FILE__, __LINE__);
//    }

    while (cpusStarted < pInfo->lapic_id){
        __asm__ volatile("nop");
//        e9_printf("doing nothing");
    }

//    __asm__ volatile("hlt");
    cpusStarted++;
//    e9_printf("entering sleep");
    while (true){
        ;;
    }
}