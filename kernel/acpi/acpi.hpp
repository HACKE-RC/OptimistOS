#ifndef PAGING_CPP_ACPI_HPP
#define PAGING_CPP_ACPI_HPP

#include <cstdint>
#include "../boot.h"
#include "../lib/string.hpp"
#include "../paging/paging.hpp"
// Forward declaration of ioAPICInfo


struct rsdpInfo{
    char signature[8];
    uint8_t checksum;
    char oemID[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} __attribute__((packed));

struct xsdpInfo {
    char signature[8];
    uint8_t checksum;
    char oemID[6];
    uint8_t revision;
    uint32_t rsdtAddress;      // deprecated since version 2.0

    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

struct SDT{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemID[6];
    char oemTableID[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creator_revision;
} __attribute__((packed));


struct RSDT{
    struct SDT systemDescriptorTable;
    char table[];
};

struct XSDT{
    struct SDT systemDescriptorTable;
    char table[];
};

struct xsdtInfo{
    bool inUse;
    xsdpInfo *xsdpInformation;
    XSDT *rootXSDT;
};

void *acpiFindTable(const char* name);
extern xsdtInfo xsdtInformation;
uintptr_t initACPI();
extern RSDT *rootRSDT;

#endif //PAGING_CPP_ACPI_HPP
