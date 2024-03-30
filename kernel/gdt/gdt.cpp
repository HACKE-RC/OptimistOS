#include "gdt.hpp"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

struct gdtEntry gdtEntries[7];
struct gdtPtrStruct gdtPtr;

void initGDT() {
    // Null descriptor
    setGDTGate(0, 0, 0, 0, 0);

    // 16-bit code descriptor. Base = 0, limit = 0xffff. Readable.
    setGDTGate(1, 0, 0xFFFF, 0x9A, 0x00);

    // 16-bit data descriptor. Base = 0, limit = 0xffff. Writable.
    setGDTGate(2, 0, 0xFFFF, 0x92, 0x00);

    // 32-bit code descriptor. Base = 0, limit = 0xffffffff. Readable.
    setGDTGate(3, 0, 0xFFFF, 0x9A, 0xCF);

    // 32-bit data descriptor. Base = 0, limit = 0xffffffff. Writable.
    setGDTGate(4, 0, 0xFFFF, 0x92, 0xCF);

    // 64-bit code descriptor. Base and limit irrelevant. Readable.
    setGDTGate(5, 0, 0, 0x9A, 0x20);

    // 64-bit data descriptor. Base and limit irrelevant. Writable.
    setGDTGate(6, 0, 0, 0x92, 0xA0);
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