#include "freeList.hpp"

fBlock* head;

fBlock* initFreeList(){
    bootInfo bootInformation = getBootInfo();
    Memory availableMemory = bootInformation.memory;
    fBlock *freeBlock = reinterpret_cast<fBlock*>(availableMemory.freeMemStart);
//    not subtracting the size of the struct because we are going to zero it out
//    anyway before returning it
    freeBlock->size = availableMemory.freeMemSize;
    freeBlock->next = nullptr;
    freeBlock->inUse = false;
    freeBlock->prevSize = 0;
    head = freeBlock;
    return head;
}