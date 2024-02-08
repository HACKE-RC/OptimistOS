#ifndef PAGING_CPP_LAPIC_HPP
#define PAGING_CPP_LAPIC_HPP
#include "../lib/memory.hpp"
#include "../paging/pageFrameAllocator.hpp"

#define LAPIC_PPR 0x00a0

#define LAPIC_ICRLO 0x0300
#define LAPIC_ICRHI 0x0310

#define LAPIC_ICINI 0x0500
#define LAPIC_ICSTR 0x0600

#define LAPIC_ICEDGE 0x0000

#define LAPIC_ICPEND 0x00001000
#define LAPIC_ICPHYS 0x00000000
#define LAPIC_ICASSR 0x00004000
#define LAPIC_ICSHRTHND 0x00000000
#define LAPIC_ICDESTSHIFT 24

#define LAPIC_ICRAIS 0x00080000
#define LAPIC_ICRAES 0x000c0000

extern void initLAPIC();

extern void lapicWrite(uint32_t reg, uint32_t value);
extern void lapicWrite(uint64_t reg, uint64_t value);
extern uint32_t lapicRead(uint32_t reg);
uint32_t lapicGetID();

#endif
