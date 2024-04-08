#include "madt.hpp"
ioAPICInfo* madtInformationList[128] = {0};
madtIso* madtISOList[128] = {0};
madtInfo madtInformation{};
uint32_t madtISOLen;

void initMADT(){
    MADT* madt = (MADT*)acpiFindTable("APIC");
    if (madt == nullptr){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    uint64_t offset = 0;
    int currentIndex = 0;



    while (true){
        if (offset > (madt->length - sizeof(madtInformation))){
            break;
        }

        madtInformation.ioAPICLen = madtInformation.madtISOLen = 0;

        madtEntry *entry = (madtEntry*)(madt->table + offset);

        switch (entry->type) {
            case 0:
                currentIndex++;
                break;
            case 1:
                madtInformationList[madtInformation.ioAPICLen++] = (ioAPICInfo*)(entry);
                break;
            case 2:
                madtISOList[madtInformation.madtISOLen++] = (madtIso*)entry;
                break;
            case 5:
                madtInformation.lapicAddr = (uint64_t*)(uint64_t*)((lapicAddr*)entry)->physLapic;
                break;
        }

        offset += entry->length;
    }
}
