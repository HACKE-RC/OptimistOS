#include "gdt.hpp"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

gdtEntry gdt[] = {
        {0, 0, 0, 0, 0, 0},
        {0xffff, 0, 0, 0x9a, 0x80, 0},
        {0xffff, 0, 0, 0x92, 0x80, 0},
        {0xffff, 0, 0, 0x9a, 0xcf, 0},
        {0xffff, 0, 0, 0x92, 0xcf, 0},
        {0, 0, 0, 0x9a, 0xa2, 0},
        {0, 0, 0, 0x92, 0xa0, 0}, 
        {0, 0, 0, 0xF2, 0, 0},
        {0, 0, 0, 0xFA, 0x20, 0},
};

struct gdtPtrStruct gdtPtr;

void initGDT() {
    gdtPtr.limit = (sizeof(gdtEntry) * 10) - 1;
    gdtPtr.base = (uintptr_t)&gdt;
    gdtFlush(&gdtPtr);
    e9_printf("done");
}
