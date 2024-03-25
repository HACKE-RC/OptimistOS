#include "acpi.hpp"

RSDT *rootRSDT = nullptr;
xsdtInfo xsdtInformation{};

void *acpiFindTable(const char* name){
    if (rootRSDT != nullptr){
//      may need to convert to physaddr
        auto *rsdt = (RSDT*) toVirtualAddr(rootRSDT);
        uint32_t entries = (rsdt->systemDescriptorTable.length - sizeof(rsdt->systemDescriptorTable)) / 4;

        for (uint32_t i = 0; i < entries; i++){
//           may need to convert to virtaddr
            SDT* sdt = (SDT*)(void*)(*((uint32_t*)rsdt->table + i));
            if ((memorycmp(sdt->signature, name, 4))){
                return sdt;
            }
        }
        return nullptr;
    }

//   xsdt
    XSDT* xsdt = (XSDT*)toVirtualAddr((void*)xsdtInformation.rootXSDT);
    uint32_t entries = (xsdt->systemDescriptorTable.length - sizeof(xsdt->systemDescriptorTable)) / 8;

    for (int i = 0; i < entries; ++i) {
        SDT* sdt = (SDT*)toPhysicalAddr((void*)(*((uint64_t*)xsdt->table + i)));
        if (memorycmp(sdt->signature, name, 4)) {
            return sdt;
        }
    }

    return nullptr;
}

uintptr_t initACPI(){
    auto *rsdpInformation = (rsdpInfo*)rsdpRequest.response->address;

    if (!(memorycmp(rsdpInformation->signature, "RSD PTR ", 8))){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    if (rsdpInformation->revision != 0){
//      version 2 or above
        xsdtInformation.xsdpInformation = (xsdpInfo*)rsdpInformation;
        xsdtInformation.rootXSDT = (XSDT*)toPhysicalAddr((void*)xsdtInformation.xsdpInformation->xsdtAddress);
        xsdtInformation.inUse = true;
        return xsdtInformation.xsdpInformation->xsdtAddress;
    }

    rootRSDT = (RSDT*)toPhysicalAddr((void*)rsdpInformation->rsdtAddress);
    return rsdpInformation->rsdtAddress;
}