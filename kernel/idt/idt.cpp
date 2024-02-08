#include "idt.hpp"

idtEntry idtEntries[256];
idtPtrStruct idtPtr;

void idtInit(){
    idtPtr.limit = (sizeof(idtEntry) * 256) - 1;
    idtPtr.base = (uint64_t)idtEntries;

    __asm__ volatile("lidt (%0)"
                    :
                    : "r"(&idtPtr));
}


void idtInitAgain(){
    idtPtr.limit = (sizeof(idtEntry) * 256) - 1;
    idtPtr.base = (uint64_t)idtEntries;

    __asm__ volatile("lidt (%0)"
            :
            : "r"(&idtPtr));
    __asm__ volatile ("sti");
}

void setIDTGate(uint8_t gateNumber, uint64_t gateAddr){
    uint16_t offsetLow = (uint16_t)(gateAddr & 0xFFFF);
    uint16_t offsetMiddle = (uint16_t)((gateAddr >> 16 ) & 0xFFFF);
    uint32_t offsetHigh = (uint32_t)((gateAddr >> 32) & 0xFFFFFFFF);

    idtEntry entry = {
        .offsetLow = offsetLow,
        .segmentSelector = 0x08,
        .reserved = 0,
        .attributes = 0x8E,
        .offsetMiddle = offsetMiddle,
        .offsetHigh = offsetHigh,
        .reserved2 = 0
    };

    idtEntries[gateNumber] = entry;
}