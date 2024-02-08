#ifndef PAGING_CPP_MADT_HPP
#define PAGING_CPP_MADT_HPP

// Forward declaration of ioAPICInfo
struct ioAPICInfo;
#include "ioapic.hpp"
#include "acpi.hpp"

extern void initMADT();

struct MADT {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemID[6];
    char oemTableID[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;

    /* MADT Specs */
    uint32_t lapicAddress;
    uint32_t flags;

    char table[];
};

struct madtEntry{
    uint8_t type;
    uint8_t length;
};

struct ioAPICInfo{
    madtEntry un;
    uint8_t apicID;
    uint8_t reserved;
    uint32_t apicAddr;
    uint32_t gsiBase;
};

struct madtInfo{
    uint32_t ioAPICLen = 0;
    uint32_t madtISOLen = 0;
    uint64_t *lapicAddr = nullptr;
};

struct lapicAddr {
    madtEntry un;
    uint16_t resv;
    uint64_t physLapic;
};

struct madtIso{
    madtEntry un;
    uint8_t busSrc;
    uint8_t irqSrc;
    uint32_t gsi;
    uint16_t flags;
} ;

extern ioAPICInfo* madtInformationList[];
extern madtIso* madtISOList[128];
extern madtInfo madtInformation;

#endif
