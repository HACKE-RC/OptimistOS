#include "gdt.hpp"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

gdtEntry gdt[] = {
        {0, 0, 0, 0, 0, 0}, // null
        {0xffff, 0, 0, 0x9a, 0x80, 0}, // 16-bit code
        {0xffff, 0, 0, 0x92, 0x80, 0}, // 16-bit data
        {0xffff, 0, 0, 0x9a, 0xcf, 0}, // 32-bit code
        {0xffff, 0, 0, 0x92, 0xcf, 0}, // 32-bit data
        {0, 0, 0, 0x9a, 0xa2, 0}, // 64-bit code
        {0, 0, 0, 0x92, 0xa0, 0}, // 64-bit data
        {0, 0, 0, 0xF2, 0, 0}, // user data
        {0, 0, 0, 0xFA, 0x20, 0}, // user code
};

struct gdtPtrStruct gdtPtr;

void initGDT() {
    gdtPtr.limit = (sizeof(gdtEntry) * 10) - 1;
    gdtPtr.base = (uintptr_t)&gdt;
    gdtFlush(&gdtPtr);
    e9_printf("done");
}
