#include "ioapic.hpp"
#include "../boot.h"

void initIOAPIC(){
    ioAPICInfo *IOAPIC = madtInformationList[0];
    uint32_t value = ioAPICRead(IOAPIC, IOAPIC_VER);
    uint32_t count = ((value >> 16) & 0xFF);

    if ((ioAPICRead(IOAPIC, 0) >> 24) != IOAPIC->apicID){
        haltAndCatchFire( __FILE__, __LINE__);
    }

    e9_printf("\nIOAPICREAD IOAPIC APICID: %d\n", IOAPIC->apicID);
    e9_printf("\nIOAPICREAD IOAPIC_VER: %d\n", value);
    e9_printf("\nIOAPIC COUNT: %d\n", count);

    for (uint32_t i = 0; i <= count; i++){
        ioAPICWrite(IOAPIC, IOAPIC_REDTBL + 2 * i, 0x00010000 | (32 + i));
        ioAPICWrite(IOAPIC, IOAPIC_REDTBL + 2 * i + 1, 0);
    }
}

void ioapic_set_entry(ioAPICInfo* ioapic, uint8_t idx, uint64_t data) {
    ioapic_write(ioapic, (uint8_t (IOAPIC_REDTBL + idx * 2), (uint32_t)data);
    ioapic_write(ioapic, (uint8_t (IOAPIC_REDTBL + idx * 2 + 1), (uint32_t)(data >> 32));
}

uint64_t ioapic_gsi_count(ioAPICInfo* ioapic) {
    return (ioAPICRead(ioapic, 1) & 0xff0000) >> 16;
}

ioAPICInfo* ioapic_get_gsi(uint32_t gsi) {
    for (uint64_t i = 0; i < madtInformation.ioAPICLen; i++) {
        ioAPICInfo* ioapic = madtInformationList[i];
        if (ioapic->gsiBase <= gsi && ioapic->gsiBase + ioapic_gsi_count(ioapic) > gsi)
            return ioapic;
    }
    return NULL;
}

void ioapic_redirect_gsi(uint32_t lapic_id, uint8_t vec, uint32_t gsi, uint16_t flags, bool mask) {
    ioAPICInfo* ioapic = ioapic_get_gsi(gsi);

    uint64_t redirect = vec;

    if ((flags & (1 << 1)) != 0) {
        redirect |= (1 << 13);
    }

    if ((flags & (1 << 3)) != 0) {
        redirect |= (1 << 15);
    }

    if (mask) redirect |= (1 << 16);
    else redirect &= ~(1 << 16);

    redirect |= (uint64_t)lapic_id << 56;

    uint32_t redir_table = (gsi - ioapic->gsiBase) * 2 + 16;
    ioAPICWrite(ioapic, redir_table, (uint32_t)redirect);
    ioAPICWrite(ioapic, redir_table + 1, (uint32_t)(redirect >> 32));
}

void ioapic_redirect_irq(uint32_t lapic_id, uint8_t vec, uint8_t irq, bool mask) {
    uint8_t idx = 0;
    madtIso* iso;

    while (idx < madtInformation.madtISOLen) {
        iso = madtISOList[idx];
        if (iso->irqSrc == irq) {
            ioapic_redirect_gsi(lapic_id, vec, iso->gsi, iso->flags, mask);
            return;
        }
        idx++;
    }

    ioapic_redirect_gsi(lapic_id, vec, irq, 0, mask);
}


uint32_t getRedirectIRQ(uint8_t irq) {
    uint8_t idx = 0;
    madtIso* iso;

    while (idx < madtInformation.madtISOLen) {
        iso = madtISOList[idx];
        if (iso->irqSrc == irq) {
            return iso->gsi;
        }
        idx++;
    }

    return irq;
}


uint32_t ioAPICRead(ioAPICInfo* entry, int reg){
//  selects and then reads the register
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_REGSEL)), (uint32_t)reg);
    return mmioRead32(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_IOWIN)));
}

void ioAPICWrite(ioAPICInfo* entry, int reg, uint32_t value){
//  selects and then reads the register
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_REGSEL)), (uint32_t)reg);
    mmioWrite(toVirtualAddr((void*)(entry->apicAddr + IOAPIC_IOWIN)), value);
}