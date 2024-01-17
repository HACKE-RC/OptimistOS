#include "freeList.hpp"

fBlock* head;

fBlock* initFreeList(){
    bootInfo bootInformation = getBootInfo();
    Memory availableMemory = bootInformation.memory;
    fBlock *freeBlock = (struct fBlock*)availableMemory.freeMemStart;
    freeBlock->size = availableMemory.freeMemSize - (sizeof(struct fBlock));
    freeBlock->next = nullptr;
    freeBlock->inUse = false;
    freeBlock->prevSize = 0;
    head = freeBlock;
    return head;
}