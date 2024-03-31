#ifndef OPTIMISTOS_IDT_HPP
#define OPTIMISTOS_IDT_HPP
#include <cstdint>

#include "isr.hpp"
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

extern void idtInit();
void idtInitAgain();
void setIDTGate(uint8_t gateNumber, uintptr_t gateAddr);

extern idtEntry idtEntries[256]; // max entries supported are 256
extern idtPtrStruct idtPtr;
void load_idt(uint64_t);
void trigger_interupt(uint64_t a);
void set_idt_gate(int num, uint64_t base, uint16_t sel, uint8_t flags);
void init_idt();
void irq_register(uint8_t irq, void *handler);
void irq_deregister(uint8_t irq);
extern void i8259_SendEndOfInterrupt(int irq);
extern "C" void excp_handler(cpuRegisters frame);
#endif //OPTIMISTOS_IDT_HPP