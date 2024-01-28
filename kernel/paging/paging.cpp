#include "paging.hpp"
#include "../boot.h"
#include "../../limine/limine.h"
bootInfo bootInformation{};

PageTable* PML4 = nullptr;

void initPaging(){
    bootInformation = getBootInfo();
    PML4 = (struct PageTable*)allocateFrame(4096);
    auto cr3 = (uintptr_t)PML4;
   
    e9_printf("\nPhysical Address of PML4 #2: %x", PML4);
    e9_printf("\nLast call!");

    PML4 = (struct PageTable*)toVirtualAddr(PML4);

    for (uint64_t i = 0; i < (4 * _1GB); i += _1GB) {
        map(i, (void*)i, (pageTableFlag)(ReadWrite |  Present | LargerPages));
    }

    for (size_t i = 0; i < memmap_request.response->entry_count; i++){
        limine_memmap_entry *mmap = memmap_request.response->entries[i];

        uint64_t start = roundDown(mmap->base, 4 * _1GB);
        uint64_t end = roundUp(mmap->base + mmap->length, 4 * _1GB);

        if (end < (4 * _1GB)){
            continue;
        }

        auto size = end - start;
        auto pageSize = 4 * _1GB;
        auto roundedSize = roundDown(size, 4 * _1GB);
        auto difference = size - roundedSize;

        for (uint64_t k = start; k < (start + roundedSize); k += 4 * _1GB){
            if (k < (4 * _1GB)){
                continue;
            }

            map(k, (void*)(k), (pageTableFlag)(ReadWrite | Present | LargerPages));
        }

        start += roundedSize;

        for (uint64_t k = start; k < (start + difference); k += _4KB){
            if (k < (4 * _1GB)){
                continue;
            }

            map(k, (void*)k, (pageTableFlag)(ReadWrite | Present));
        }
    }

    for (size_t i = 0; i < kernelFileRequest.response->kernel_file->size; i += (4 * _1GB)) {
        uint64_t physicalAddr = kernelMemoryRequest.response->physical_base + i;
        uint64_t virtualAddr = kernelMemoryRequest.response->virtual_base + i;

        map(physicalAddr, (void*)virtualAddr, (pageTableFlag)(ReadWrite | UserOrSuperuser | Present));
    }

    setCr3(cr3);
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
    size_t pml4Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 39)) >> 39;
    size_t pml3Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 30)) >> 30;
    size_t pml2Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 21)) >> 21;
    size_t pml1Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 12)) >> 12;

    if (PML4 == nullptr){
        return nullptr;
    }

    PageTable *PML3, *PML2, *PML1;

    PML3 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML4->entries[pml4Entry], true));
    if (PML3 == nullptr){
        return nullptr;
    }

    if (flags & LargerPages){
        return &PML3->entries[pml3Entry];
    }

    PML2 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML3->entries[pml3Entry], true));
    if (PML2 == nullptr){
        return nullptr;
    }

    PML1 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML2->entries[pml2Entry], true)); if (PML1 == nullptr){
        return nullptr;
    }

    return &PML1->entries[pml1Entry];
}

bool map(uintptr_t physicalAddr, void* virtualAddr, pageTableFlag flags){
    auto *entry = (PageDirectoryEntry*)(virtualAddrToPTE(virtualAddr, true, flags));

    if (entry != nullptr){
        entry->setFlag(flags, true);
        entry->setAddress(physicalAddr);
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
