#include "freeList.hpp"

static fBlock* head;

#define PAGESIZE 4096 // 4KBs

//std::unordered_map<void*, size_t> addressSizeHT;

fBlock* initFreeList(){
    bootInfo bootInformation = getBootInfo();
    Memory availableMemory = bootInformation.memory;
    auto *freeBlock = (fBlock*)(availableMemory.freeMemStart);
    freeBlock = (fBlock*)(toVirtualAddr((void *) freeBlock));

    //    not subtracting the size of the struct because we are going to zero it out
    //    anyway before returning it
    freeBlock->size = availableMemory.freeMemSize;
    freeBlock->next = nullptr;
    freeBlock->inUse = false;
    freeBlock->prevSize = 0;

    head = freeBlock;
    return head;
}

//fBlock* initFreeListProcess(){
//    uint64_t currentPML4 = readCr3();
//    auto *freeBlock = (fBlock*) toVirtualAddr((void*)currentPML4);
////    freeBlock->size = availableMemory.freeMemSize;
//    freeBlock->next = nullptr;
//    freeBlock->inUse = false;
//    freeBlock->prevSize = 0;
//
//    head = freeBlock;
//
//    e9_printf("\nfreeList init: %x\n", freeBlock);
//    return head;
//}