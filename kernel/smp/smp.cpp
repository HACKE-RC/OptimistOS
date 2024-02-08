#include "smp.hpp"
int bspLAPICID = 0;
int cpuCount;

cpuInfo* cpuInformation[256];

void initSMP(){
    struct limine_smp_response *smpResponse = (struct limine_smp_response *)smpRequest.response;

    cpuCount = smpResponse->cpu_count;
    bspLAPICID = smpResponse->bsp_lapic_id;

    e9_printf("num cpus: %d\n", cpuCount );

    cpuInfo *cpu0Information = (cpuInfo *)allocateFrame(sizeof(cpuInfo));
    cpu0Information->lock = false;
    cpu0Information->lapicID = 0;
    cpu0Information->currentProcess = 0;
    cpu0Information->processIndex = cpu0Information->processSize = 0;
    cpu0Information->lastIdleTime = cpu0Information->totalTime = 0;
    cpu0Information->processPRCount = 0;

    cpuInformation[0] = cpu0Information;

    for (uint64_t i = 0; i < cpuCount; i++){
        smpResponse->cpus[i]->goto_address = printHello;
    }

}

void printHello(limine_smp_info *pInfo){
    e9_printf("Hello from CPU %d\n", pInfo->processor_id);
}