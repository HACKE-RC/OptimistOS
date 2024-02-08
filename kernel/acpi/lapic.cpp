#include "lapic.hpp"

void initLAPIC(){
    lapicWrite(0xf0, lapicRead(0xf0) | 0x100);
}

void lapicWrite(uint32_t reg, uint32_t value){
    mmioWrite(toVirtualAddr((void*)(0xfee00000 + reg)), value);
}

uint32_t lapicRead(uint32_t reg){
    return mmioRead32(toVirtualAddr((void*)(0xfee00000 + reg)));
}

uint32_t lapicGetID() {
    return lapicRead(0x0020) >> LAPIC_ICDESTSHIFT;
}