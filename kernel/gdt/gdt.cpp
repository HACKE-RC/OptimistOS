#include "gdt.hpp"
#include "kernelEntry.hpp"

extern "C" void gdtFlush(void* addr_t);

//struct gdtEntry gdtEntries[10];
//struct gdtPtrStruct gdtPtr;
extern "C" void tssFlush();

static tss_t tss;
static gdt_desc_t gdt[10];
static gdtr_t gdtr;

void initGDT(void)
{
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0, 0x9A, 0xA0);
    gdt_set_entry(2, 0, 0, 0x92, 0xA0);
    gdt_set_entry(3, 0, 0, 0, 0);
    gdt_set_entry(4, 0, 0, 0x92, 0xA0);
    gdt_set_entry(5, 0, 0, 0x9A, 0xA0);
    gdt_set_entry(6, 0, 0, 0x92, 0xA0);
    gdt_set_entry(7, 0, 0, 0x9A, 0xA0);
    gdt_set_entry(8, 0, 0, 0x89, 0xA0);
    gdt_set_entry(9, 0, 0, 0, 0);

    for (uint64_t i = 0; i < sizeof(tss); i++)
        ((uint8_t *)(void *)&tss)[i] = 0;

    uint64_t tss_base = ((uint64_t)&tss);

    gdt[8].base_lo = tss_base & 0xFFFF;
    gdt[8].base_mid = (tss_base >> 16) & 0xFF;
    gdt[8].base_hi = (tss_base >> 24) & 0xFF;
    gdt[8].limit = sizeof(tss);
    gdt[9].limit = (tss_base >> 32) & 0xFFFF;
    gdt[9].base_lo = (tss_base >> 48) & 0xFFFF;

    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset = (uint64_t)&gdt;

    gdtFlush(&gdtr);

}

void gdt_set_entry(int entry, uint16_t limit, uint32_t base, uint8_t type,
                   uint8_t flags)
{
    gdt[entry].limit = limit;

    gdt[entry].base_lo = (base >> 8) & 0xFF;
    gdt[entry].base_mid = (base >> 16) & 0xFF;
    gdt[entry].base_hi = (base >> 24) & 0xFF;

    gdt[entry].type = type;
    gdt[entry].flags = flags;
}
