#ifndef OPTIMISTOS_GDT_HPP
#define OPTIMISTOS_GDT_HPP
#include <stdint.h>
//struct gdtEntry {
//    uint16_t limit;
//    uint16_t baseLow;
//    uint8_t baseMiddle;
//    uint8_t access;
//    uint8_t flags;
//    uint8_t baseHigh;
//} __attribute__((packed));

//struct gdtPtrStruct{
//    uint16_t limit;
//    gdtEntry* base;
//} __attribute__((packed)) ;


#define GDT_ENTRIES 10

typedef struct {
    uint16_t limit;
    uint16_t base_lo;
    uint8_t base_mid;
    uint8_t type;
    uint8_t flags;
    uint8_t base_hi;
} __attribute__((packed)) gdt_desc_t;

typedef struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} __attribute__((packed)) tss_t;

typedef struct gdt_t {
    gdt_desc_t entry[10];
} __attribute__((packed)) __attribute__((aligned(0x1000))) gdt_t;

typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdtr_t;

void gdt_set_entry(int entry, uint16_t limit, uint32_t base, uint8_t type,
                   uint8_t flags);

void initGDT();
void setGDTGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);


#endif //OPTIMISTOS_GDT_HPP
