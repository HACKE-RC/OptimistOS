#include "gdt.hpp"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

struct gdtEntry gdtEntries[7];
struct gdtPtrStruct gdtPtr;

void initGDT() {
    // Null descriptor
    setGDTGate(0, 0, 0, 0, 0);

    gdtEntries[1].limit = 0xffff;
    gdtEntries[1].access = 0x9a;
    gdtEntries[2].limit = 0xffff;
    gdtEntries[2].access = 0x92;
    gdtEntries[3].limit = 0xffff;
    gdtEntries[3].access = 0x9A;
    gdtEntries[3].flags = 0xcf;
    gdtEntries[4].limit = 0xffff;
    gdtEntries[4].access = 0x92;
    gdtEntries[4].flags= 0xcf;
    gdtEntries[5].limit = 0;
    gdtEntries[5].access = 0x9a;
    gdtEntries[5].flags = 0x20;
    gdtEntries[6].access = 0x92;
    gdtEntries[6].limit = 0;
    gdtEntries[6].flags = 0x00;

    // 16-bit code descriptor. Base = 0, limit = 0xffff. Readable.
//    setGDTGate(1, 0, 0xFFFF, 0x9A, 0x00);

    // 16-bit data descriptor. Base = 0, limit = 0xffff. Writable.
//    setGDTGate(2, 0, 0xFFFF, 0x92, 0x00);

    // 32-bit code descriptor. Base = 0, limit = 0xffffffff. Readable.

    // 32-bit data descriptor. Base = 0, limit = 0xffffffff. Writable.

    // 64-bit code descriptor. Base and limit irrelevant. Readable.

    // 64-bit data descriptor. Base and limit irrelevant. Writable.
    gdtPtr.limit = (sizeof(struct gdtEntry) * 7) - 1;
    gdtPtr.base = gdtEntries;
    asm volatile ("lgdt %0" : : "m"(gdtPtr) : "memory");
}

void setGDTGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    gdtEntries[num].baseLow = (base & 0xFFFF);
    gdtEntries[num].baseMiddle = (base >> 16) & 0xFF;
    gdtEntries[num].baseHigh = (base >> 24) & 0xFF;
    gdtEntries[num].limit = (limit & 0xFFFF);
    gdtEntries[num].flags = (limit >> 16) & 0x0F;
    gdtEntries[num].flags |= (gran & 0xF0);
    gdtEntries[num].access = access;
}