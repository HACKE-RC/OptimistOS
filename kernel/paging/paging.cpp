#include "paging.hpp"
bootInfo bootInformation{};

PageTable* PML4 = nullptr;

void initPaging(){
    bootInformation = getBootInfo();
    PML4 = (struct PageTable*)allocateFrame(4096);
    auto cr3 = (uintptr_t)PML4;
   
    e9_printf("\nPhysical Address of PML4 #2: %x", PML4);
    e9_printf("\nLast call!");

    PML4 = (struct PageTable*)toVirtualAddr(PML4);
//100000000

    for (uint64_t i = 0; i < (_1GB * 4); i += _1GB) {
        map(i, (void*)i, (pageTableFlag)(ReadWrite |  Present | LargerPages));
    }

//    setCr3(cr3);
    e9_printf("\nLast call 2!");
    e9_printf("\nPML4 setup complete");
    e9_printf("\nreg value: %x", readCr3());
}

uintptr_t getNextLevelPointer(PageDirectoryEntry& entry, bool allocate){
    uintptr_t nextLevelPointer = 0;

    if (entry.isValid()){
        nextLevelPointer = entry.getAddress();
    }
    else if (allocate){
        nextLevelPointer = allocateFrame(0x1000);
        entry.setAddress(nextLevelPointer);
        entry.setFlag(ReadWrite, true);
        entry.setFlag(UserOrSuperuser, true);
        entry.setFlag(Present, true);
    }

    if (nextLevelPointer != 0){
        return nextLevelPointer;
    }

    return -1;
}
PageDirectoryEntry *virtualAddrToPTE(void* virtualAddr, bool allocate, pageTableFlag flags){
    e9_printf("converting virtual addr to pte");
    size_t pml4Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 39)) >> 39;
    size_t pml3Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 30)) >> 30;
    size_t pml2Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 21)) >> 21;
    size_t pml1Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 12)) >> 12;

    if (PML4 == nullptr){
        return nullptr;
    }

    PageTable *PML3, *PML2, *PML1;

    e9_printf("PML3 setup start\n");
    PML3 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML4->entries[pml4Entry], true));
    if (PML3 == nullptr){
        return nullptr;
    }
    e9_printf("PML3 setup end\n");

    if (flags & LargerPages){
        return &PML3->entries[pml3Entry];
    }

    e9_printf("PML2 setup start\n");
    PML2 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML3->entries[pml3Entry], true));
    if (PML2 == nullptr){
        return nullptr;
    }
    e9_printf("PML2 setup end\n");

    e9_printf("PML1 setup start\n");
    PML1 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML2->entries[pml2Entry], true)); if (PML1 == nullptr){
        return nullptr;
    }
    e9_printf("PML1 setup end\n");

    return &PML1->entries[pml1Entry];
}

bool map(uintptr_t physicalAddr, void* virtualAddr, pageTableFlag flags){
    e9_printf("calling virtualAddrToPTE\n");
    auto *entry = (PageDirectoryEntry*)(virtualAddrToPTE(virtualAddr, true, flags));
    e9_printf("recieved entry at: %x\n", entry);

    if (entry != nullptr){
        entry->setFlag(flags, true);
        entry->setAddress(physicalAddr);
        e9_printf("entry flags set\n");
        return true;
    }

    return false;
}

void setCr3(uint64_t value) {
    asm volatile("movq %0, %%cr3" : : "r" (value));
}

uint64_t readCr3() {
    uint64_t cr3Value;
    __asm__ volatile ("mov %%cr3, %0" : "=r" (cr3Value));
    return cr3Value;
}
