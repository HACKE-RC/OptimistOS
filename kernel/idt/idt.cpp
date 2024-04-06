#include "idt.hpp"

idtEntry idtEntries[256];
idtPtrStruct idtPtr;
bool wasInit = false;

void idtInit(){
//    __asm__ volatile ("cli");
    idtPtr.limit = ((uint16_t)sizeof(idtEntry) * 256) - 1;
    idtPtr.base = (uint64_t)&idtEntries;

    asm volatile ("lidt %0" :: "memory"(idtPtr));
    wasInit = true;
//    __asm__ volatile ("sti");
}

void idtInitAgain(){
    __asm__ volatile ("cli");
    __asm__ volatile("lidt (%0)"
            :
            : "r"(&idtPtr));
    __asm__ volatile ("sti");
}

void setIDTGate(uint8_t gateNumber, uintptr_t gateAddr){
    uint16_t offsetLow = (uint64_t)(gateAddr & 0xFFFF);
    uint16_t offsetMiddle = (uint64_t)((gateAddr >> 16 ) & 0xFFFF);
    uint32_t offsetHigh = (uint64_t)((gateAddr >> 32) & 0xFFFFFFFF);

    idtEntry entry = {
        .offsetLow = offsetLow,
        .segmentSelector = 0x28,
        .reserved = 0,
        .attributes = (uint8_t)0x8E,
        .offsetMiddle = offsetMiddle,
        .offsetHigh = offsetHigh,
        .reserved2 = 0
    };
    idtEntries[gateNumber] = entry;
}