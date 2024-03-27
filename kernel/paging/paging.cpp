#include "paging.hpp"
#include "../boot.h"
#include "../../limine/limine.h"
#include "../lib/lock.hpp"

bootInfo bootInformation{};

uint64_t hhdmOffset = 0;
PageTable* PML4 = nullptr;
PageTable* kernelPML4 = nullptr;

static uint32_t mutex = 0;

void init(){
    PML4 = (struct PageTable*)(allocateFrame(0x1000));
    if (kernelPML4 == nullptr){
        kernelPML4 = PML4;
    }
}

extern "C" {
    extern char KERNEL_BLOB_BEGIN[];
    extern char KERNEL_BLOB_SIZE[];
    extern char KERNEL_TEXT_BEGIN[];
    extern char KERNEL_TEXT_SIZE[];
    extern uint64_t KERNEL_RODATA_BEGIN[];
    extern uint64_t KERNEL_RODATA_SIZE[];
    extern uint64_t KERNEL_DATA_BEGIN[];
    extern uint64_t KERNEL_DATA_SIZE[];
}

void initPaging(){
    lock(mutex);
    init();

    bootInformation = getBootInfo();
    hhdmOffset = hhdmRequest.response->offset;

//     map the first 4gb
    for (uint64_t i = 0; i < (4 * _1GB); i += (2 * _1MB)){
        (map(i, (void*)(i), (pageTableFlag)(ReadWrite | Present | LargerPages), (2 * _1MB)));
        (map(i, (void*)(i + hhdmOffset), (pageTableFlag)(ReadWrite | Present | LargerPages), (2 * _1MB)));
    }

    for (size_t i = 0; i < memmapRequest.response->entry_count; i++){
        limine_memmap_entry *mmap = memmapRequest.response->entries[i];

        uint64_t start = roundDown(mmap->base, _4KB);
        uint64_t end = roundUp(mmap->base + mmap->length, _4KB);

        if (start < (4 * _1GB)){
            continue;
        }

        auto size = end - start;
        auto [pageSize, sizeFlags] = requiredSize(size);
        auto roundedSize = roundDown(size, pageSize);
        auto difference = size - roundedSize;

        for (uint64_t k = start; k < (start + roundedSize); k += pageSize){
            if (k < (4 * _1GB)){
                continue;
            }
            map(k, (void*)(k + hhdmOffset), (pageTableFlag)(ReadWrite | Present | sizeFlags), pageSize);
        }

        start += roundedSize;

        for (uint64_t k = start; k < (start + difference); k += pageSize){
            if (!(map(k, (void*)(k + hhdmOffset), (pageTableFlag)(ReadWrite | Present | sizeFlags), pageSize))){
                haltAndCatchFire(__FILE__, __LINE__);
            }
        }

    }

    uint64_t physicalBase = kernelMemoryRequest.response->physical_base;
    uint64_t virtualBase = kernelMemoryRequest.response->virtual_base;
    for (auto i = (uintptr_t) roundDown((uintptr_t)KERNEL_BLOB_BEGIN, _4KB); i < (uintptr_t) roundDown((uintptr_t )KERNEL_BLOB_BEGIN, _4KB) + roundUp((uintptr_t)KERNEL_BLOB_SIZE + (uintptr_t)(KERNEL_BLOB_BEGIN -
            roundDown((uintptr_t)KERNEL_BLOB_BEGIN, _4KB)) , _4KB); i += (_4KB)) {
        map(i - virtualBase + physicalBase, (void*)(i), (pageTableFlag)(ReadWrite | Present), _4KB);
    }

    if (PML4 == nullptr){
        asm volatile("hlt");
    }


    writeCrReg(3, (uint64_t)PML4);
    unlock(mutex);
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

    PML2 = (PageTable*)toVirtualAddr((void*)getNextLevelPointer(PML3->entries[pml3Entry], true, virtualAddr, pageSize));
    if (PML2 == nullptr){
        haltAndCatchFire(__FILE__, __LINE__);
    }

    if (flags & LargerPages){
        return &PML2->entries[pml2Entry];
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
        return {(2 * _1MB), LargerPages};
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

PageTable* newPML4(){
    PageTable* newPML4 = (struct PageTable*)(allocateFrame(0x1000));

    for (int i = 256; i < 512; i++){
        newPML4->entries[i] = PML4->entries[i];
    }

    return newPML4;
}