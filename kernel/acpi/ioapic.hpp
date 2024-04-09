#ifndef PAGING_CPP_IOAPIC_HPP
#define PAGING_CPP_IOAPIC_HPP

#include "../paging/pageFrameAllocator.hpp"
#include "../lib/memory.hpp"
#include "madt.hpp"

#define IOAPIC_REGSEL 0x0
#define IOAPIC_IOWIN  0x10

#define IOAPIC_ID     0x0
#define IOAPIC_VER    0x01
#define IOAPIC_ARB    0x02
#define IOAPIC_REDTBL 0x10

extern void initIOAPIC();
extern uint32_t ioAPICRead(ioAPICInfo* entry, int reg);
extern void ioAPICWrite(ioAPICInfo* entry, int reg, uint32_t value);
extern void redirectGSI(uint32_t lapicID, uint8_t vec, uint32_t gsi, uint16_t flags, bool mask);
extern uint32_t getRedirectIRQ(uint8_t irq);
extern void redirectIRQ(uint32_t lapicID, uint8_t vec, uint8_t irq, bool mask);
extern void redirectGSI(uint32_t lapicID, uint8_t vec, uint32_t gsi, uint16_t flags, bool mask);
extern ioAPICInfo* getGSI(uint32_t gsi);
uint64_t getGSICount(ioAPICInfo* ioapic);
extern void registerIRQ(uint8_t vec, void* handler);
extern void unregisterIRQ(uint8_t vec);
void setEntry(ioAPICInfo* ioapic, uint8_t idx, uint64_t data);
#endif
