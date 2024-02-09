#include "gdt.hpp"
//#include "gdt.asm"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

struct gdtEntry gdtEntries[5];
struct gdtPtrStruct gdtPtr;

void initGDT(){
    gdtPtr.limit = (sizeof(struct gdtEntry) * 5) - 1;
    gdtPtr.base = gdtEntries;

    setGDTGate(0, 0, 0, 0, 0); // NULL gate
    setGDTGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // NULL gate
    setGDTGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // NULL gate
    setGDTGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // NULL gate
    setGDTGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // NULL gate
//    gdtFlush(&gdtPtr);
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
