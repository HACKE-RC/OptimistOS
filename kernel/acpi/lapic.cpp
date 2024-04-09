#include "lapic.hpp"
uint32_t ticksPerMS = 0;

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

void lapicIPI(uint32_t flags, uint32_t id)
{
    lapicWrite(0x310, id << 24);
    lapicWrite(0x300, flags);
}

void lapicEOI(){
    lapicWrite((uint32_t)0xB0, 0x00);
}

void lapicTimerCalibrate()
{
    lapicWrite((uint32_t)0x3E0, 0x03);
    lapicWrite(0x380, 0xFFFFFFFF);

    lapicWrite(0x320, lapicRead(0x320) & ~(1 << 16));

    asm volatile("cli");
    sleepMS(10);
    lapicWrite(0x320, lapicRead(0x320) | (1 << 16));

    ticksPerMS = (0xFFFFFFFF - lapicRead(0x390)) / 10;
}

void lapicTimer(uint8_t vector, uint64_t ms)
{
    if (ticksPerMS == 0){
        lapicTimerCalibrate();
    }

    uint64_t ticks = ticksPerMS * ms;

    lapicWrite((uint32_t)0x3E0, 0x03);
    uint64_t value = lapicRead(0x320) & ~(3 << 17);

    value |= 0 << 17;
    value &= 0xFFFFFF00;
    value |= vector;

    lapicWrite(0x320, value);
    lapicWrite(0x380, ticks ? ticks : 1);
    lapicWrite(0x320, lapicRead(0x320) & ~(1 << 16));
}