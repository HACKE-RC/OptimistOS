#include "paging.hpp"
#include "../boot.h"
#include "../../limine/limine.h"
bootInfo bootInformation{};

uint64_t hhdmOffset = 0;


void init(){
    PML4 = (struct PageTable*)(allocateFrame(0x1000));
}


void initPaging(){
    init();

    bootInformation = getBootInfo();
    hhdmOffset = hhdm_request.response->offset;

//  identity map 0 to 0x100000000
    for (uint64_t i = 0; i < (4 * _1GB); i += _1GB) {
        map((i), (void*)i, (pageTableFlag)(ReadWrite |  Present | LargerPages), _1GB);
    }
    e9_printf("first 4 gb mapping done!");
//  map 0xFFFF800000000000 - 0xFFFF800100000000 to 0x0 - 0x100000000
    for (uint64_t i = 0; i < (4 * _1GB); i += _1GB){
        map(i, (void*)(i + hhdmOffset), (pageTableFlag)(ReadWrite | Present | LargerPages), _1GB);
    }
//
    for (uint64_t i = 0; i < (2 * _1GB); i += _1GB){
        map(i, (void*)(i + 0xFFFFFFFF80000000UL), (pageTableFlag)Present, _1GB);
    }
    for (size_t i = 0; i < memmap_request.response->entry_count; i++){
        limine_memmap_entry *mmap = memmap_request.response->entries[i];

        uint64_t start = roundDown(mmap->base, _1GB);
        uint64_t end = roundUp(mmap->base + mmap->length, _1GB);

        if (end < (4 * _1GB)){
            continue;
        }

        auto size = end - start;
        auto pageSize = _1GB;
        auto roundedSize = roundDown(size, _1GB);
        auto difference = size - roundedSize;

        for (uint64_t k = start; k < (start + roundedSize); k += _1GB){
            if (k < (4 * _1GB)){
                continue;
            }

            map(k, (void*)(k + 0xFFFF800000000000UL), (pageTableFlag)(ReadWrite | Present | LargerPages), _1GB);
        }

        start += roundedSize;

        for (uint64_t k = start; k < (start + difference); k += _4KB){
            if (k < (4 * _1GB)){
                continue;
            }

            map(k, (void*)(k + 0xFFFF800000000000UL), (pageTableFlag)(ReadWrite | Present), _4KB);
        }

    }

    e9_printf("\nsecond memmap mapping done!\n");
    for (size_t i = 0; i < bootInformation.memory.kernelSize; i += (_4KB)) {
        uint64_t physicalAddr = kernelMemoryRequest.response->physical_base + i;
        uint64_t virtualAddr = kernelMemoryRequest.response->virtual_base + i;

        map(physicalAddr, (void*)virtualAddr, (pageTableFlag)(ReadWrite | UserOrSuperuser | Present ));
    }
    e9_printf("\nkernel mapping done!\n");
    if (PML4 == nullptr){
        e9_printf("pml4 empty");
        asm volatile("hlt");
    }
    e9_printf("PML4 addr: %x", PML4);
    asm_write_cr(3, PML4);
//    writeCR3(PML4);
//    setCr3(PML4);
    e9_printf("\nLast call 2!");
    e9_printf("\nPML4 setup complete");
    e9_printf("\nreg value: %x", readCr3());
}

uintptr_t getNextLevelPointer(PageDirectoryEntry& entry, bool allocate, void* virtualAddr, size_t pageSize){
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

PageDirectoryEntry *virtualAddrToPTE(void* virtualAddr, bool allocate, pageTableFlag flags, size_t pageSize){
    size_t pml4Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 39)) >> 39;
    size_t pml3Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 30)) >> 30;
    size_t pml2Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 21)) >> 21;
    size_t pml1Entry = ((uintptr_t)virtualAddr & (0x1FFULL << 12)) >> 12;

    if (PML4 == nullptr){
        return nullptr;
    }

    PageTable *PML3 = nullptr;
    PageTable *PML2 = nullptr;
    PageTable *PML1 = nullptr;

//  x is just a copy of the original to avoid overwriting to it
    auto PML4x = (PageTable*)toVirtualAddr(PML4);

    PML3 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML4x->entries[pml4Entry], true, virtualAddr, pageSize));
    if (PML3 == nullptr){
        return nullptr;
    }

    if (flags & LargerPages){
        return &PML3->entries[pml3Entry];
    }

    PML2 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML3->entries[pml3Entry], true, virtualAddr, pageSize));
    if (PML2 == nullptr){
        return nullptr;
    }

    PML1 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML2->entries[pml2Entry], true, virtualAddr, pageSize));
    if (PML1 == nullptr){
        return nullptr;
    }

    return &PML1->entries[pml1Entry];
}

bool map(uintptr_t physicalAddr, void* virtualAddr, pageTableFlag flags, size_t pageSize){
    auto *entry = (PageDirectoryEntry*)((virtualAddrToPTE(virtualAddr, true, flags, pageSize)));

    if (entry != nullptr){
        entry->setAddress(physicalAddr);
        entry->setFlag(flags, true);
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

bool isHigherHalf(uintptr_t addr) {
    return (addr >= hhdmOffset);
}

uintptr_t toHigherHalf(uintptr_t addr) {
    if (isHigherHalf(addr)){
        return addr;
    }
    else{
        return addr + hhdmOffset;
    };
}
