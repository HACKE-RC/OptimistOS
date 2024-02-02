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

//     map the first 4gb without the null page
    for (uint64_t i = 0; i < (4 * _1GB); i += _1GB){
        (map(i, (void*)(i + hhdmOffset), (pageTableFlag)(ReadWrite | Present | LargerPages), _1GB));
    }

    e9_printf("first 4 gb mapping done!\n");

    for (size_t i = 0; i < memmap_request.response->entry_count; i++){
        limine_memmap_entry *mmap = memmap_request.response->entries[i];

        uint64_t start = roundDown(mmap->base, _4KB);
        uint64_t end = roundUp(mmap->base + mmap->length, _4KB);

        auto size = end - start;
        auto [pageSize, sizeFlags] = requiredSize(size);
        e9_printf("\nend-start: %x\npageSize: %x : %d", size, pageSize, pageSize);
        auto roundedSize = roundDown(size, pageSize);
        auto difference = size - roundedSize;

        for (uint64_t k = start; k < (start + roundedSize); k += pageSize){
            map(k, (void*)(k + hhdmOffset), (pageTableFlag)(ReadWrite | Present | sizeFlags), pageSize);
        }

        start += roundedSize;

        for (uint64_t k = start; k < (start + difference); k += pageSize){
            if (!map(k, (void*)(k + hhdmOffset), (pageTableFlag)(ReadWrite | Present | sizeFlags), pageSize)){
                haltAndCatchFire(__FILE__, __LINE__);
            }
        }

    }

    e9_printf("\nsecond memmap mapping done!\n");

    for (uint64_t i = 0; i < kernelFileRequest.response->kernel_file->size; i += (_4KB)) {
        uint64_t physicalAddr = kernelMemoryRequest.response->physical_base + i;
        uint64_t virtualAddr = kernelMemoryRequest.response->virtual_base + i;

        map(physicalAddr, (void*)(virtualAddr), (pageTableFlag)(ReadWrite | Present), _4KB);
    }

    e9_printf("\nkernel mapping done!\n");

    if (PML4 == nullptr){
        e9_printf("pml4 empty");
        asm volatile("hlt");
    }

    e9_printf("PML4 addr: %x\n", PML4);
    e9_printf("\nreg value: %x\n", readCr3());

    writeCrReg(3, (uint64_t)PML4);

    e9_printf("\nLast call 2!\n");
    e9_printf("\nPML4 setup complete\n");
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
    else{
        reportError();
    }

    if (nextLevelPointer != 0){
        return nextLevelPointer;
    }
    reportError();
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

    auto PML4x = PML4;  // copy to avoid calling toPhysicalAddr later

    PageTable *PML3 = nullptr;
    PageTable *PML2 = nullptr;
    PageTable *PML1 = nullptr;

    PML4 = (PageTable*)toVirtualAddr(PML4);

    PML3 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML4->entries[pml4Entry], true, virtualAddr, pageSize));
    if (PML3 == nullptr){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    PML4 = PML4x;

    if (flags & LargerPages){
        return &PML3->entries[pml3Entry];
    }

    PML2 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML3->entries[pml3Entry], true, virtualAddr, pageSize));
    if (PML2 == nullptr){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    PML1 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML2->entries[pml2Entry], true, virtualAddr, pageSize));
    if (PML1 == nullptr){
        haltAndCatchFire(__FILE__, __LINE__);
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

std::pair<size_t, size_t> requiredSize(size_t size) {
    if (size >= _1GB){
        return {_1GB, LargerPages};
    }
    if (size >= _4KB){
        return {_4KB, 0};
    }
    return {_4KB, 0};
}

uintptr_t toHigherHalf(uintptr_t addr) {
    if (isHigherHalf(addr)){
        return addr;
    }
    else{
        return addr + hhdmOffset;
    };
}
