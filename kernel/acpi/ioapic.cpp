#include "ioapic.hpp"
#include "../boot.h"

void initIOAPIC(){
    ioAPICInfo *IOAPIC = madtInformationList[0];
    uint32_t value = ioAPICRead(IOAPIC, IOAPIC_VER);
    uint32_t count = ((value >> 16) & 0xFF);

    if ((ioAPICRead(IOAPIC, 0) >> 24) != IOAPIC->apicID){
        haltAndCatchFire( __FILE__, __LINE__);
    }

    e9_printf("\nIOAPICREAD IOAPIC APICID: %d\n", IOAPIC->apicID);
    e9_printf("\nIOAPICREAD IOAPIC_VER: %d\n", value);
    e9_printf("\nIOAPIC COUNT: %d\n", count);

    for (uint32_t i = 0; i <= count; i++){
        ioAPICWrite(IOAPIC, IOAPIC_REDTBL + 2 * i, 0x00010000 | (32 + i));
        ioAPICWrite(IOAPIC, IOAPIC_REDTBL + 2 * i + 1, 0);
    }
    return;
}


//
uint32_t ioAPICRead(ioAPICInfo* entry, int reg){
//  selects and then reads the register
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_REGSEL)), (uint32_t)reg);
    return mmioRead32(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_IOWIN)));
}

void ioAPICWrite(ioAPICInfo* entry, int reg, uint32_t value){
//  selects and then reads the register
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_REGSEL)), (uint32_t)reg);
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_IOWIN)), value);
}
void writeEOI() {
    ioAPICInfo* APIC = madtInformationList[0];
    ioAPICWrite(APIC, 0xB0, 0);
//    uint32_t* eoiRegister = (uint32_t*)(madtInformationList[0]->apicAddr + 0xB0);
//    ioAPICWrite(madtInformationList[0], 0xB0, 0);
//    *eoiRegister = 0;
}