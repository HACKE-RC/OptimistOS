#include "acpi.hpp"

RSDT *rootRSDT = nullptr;

uintptr_t initACPI(){
    auto *rsdpInformation = (rsdpInfo*)rsdpRequest.response->address;

    if (!(memorycmp(rsdpInformation->signature, "RSD PTR ", 8))){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    if (rsdpInformation->revision != 0){
//      version 2 or above
        xsdtInfo xsdtInformation{};
        xsdtInformation.xsdpInformation = (xsdpInfo*)rsdpInformation;
        xsdtInformation.rootXSDT = (XSDT*)toPhysicalAddr((void*)xsdtInformation.xsdpInformation->xsdtAddress);
        xsdtInformation.inUse = true;
        return xsdtInformation.xsdpInformation->xsdtAddress;
    }

    rootRSDT = (RSDT*)toPhysicalAddr((void*)rsdpInformation->rsdtAddress);
    return rsdpInformation->rsdtAddress;
}