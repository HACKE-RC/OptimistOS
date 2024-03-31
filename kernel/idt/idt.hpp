#ifndef OPTIMISTOS_IDT_HPP
#define OPTIMISTOS_IDT_HPP
#include <cstdint>

extern bool wasInit;

typedef struct __attribute__((packed)){
   uint16_t offsetLow;
   uint16_t segmentSelector;
   uint8_t reserved;
   uint8_t attributes;
   uint16_t offsetMiddle;
   uint32_t offsetHigh;
   uint32_t reserved2;
} __attribute__((packed)) idtEntry;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtPtrStruct;

void idtInit();
void idtInitAgain();
void setIDTGate(uint8_t gateNumber, uintptr_t gateAddr);

extern idtEntry idtEntries[256]; // max entries supported are 256
extern idtPtrStruct idtPtr;

#endif //OPTIMISTOS_IDT_HPP