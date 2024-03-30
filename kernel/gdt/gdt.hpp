#ifndef OPTIMISTOS_GDT_HPP
#define OPTIMISTOS_GDT_HPP
#include <stdint.h>
struct gdtEntry {
    uint16_t limit;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t flags;
    uint8_t baseHigh;
} __attribute__((packed));

struct gdtPtrStruct{
    uint16_t limit;
    gdtEntry* base;
} __attribute__((packed)) ;

void initGDT();
void setGDTGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);


#endif //OPTIMISTOS_GDT_HPP