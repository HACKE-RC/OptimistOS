#ifndef PAGING_CPP_LAPIC_HPP
#define PAGING_CPP_LAPIC_HPP
#include "../lib/memory.hpp"
#include "../paging/pageFrameAllocator.hpp"

#define LAPIC_ICDESTSHIFT 24

extern void initLAPIC();
extern void lapicEOI();
extern void lapicWrite(uint32_t reg, uint32_t value);
extern uint32_t lapicRead(uint32_t reg);
uint32_t lapicGetID();

#endif
