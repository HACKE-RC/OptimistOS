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

#endif
