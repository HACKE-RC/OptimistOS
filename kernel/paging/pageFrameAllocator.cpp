#include "pageFrameAllocator.hpp"

#define PAGESIZE 4096 // 4KBs

fBlock *head = nullptr;
fBlock *freeBlock = head;
size_t totalMemory;
size_t usedMemory = 0;
static bootInfo bootInformation{};

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }
    return (size + PAGESIZE - 1) & ~(PAGESIZE - 1);
}

uintptr_t toPhysicalAddr(void *addr){
    return (uintptr_t)((uintptr_t)addr - bootInformation.memory.hhdmOffset);
}

void *toVirtualAddr(void *addr){
    return (void*)((uintptr_t)addr + bootInformation.memory.hhdmOffset);
}

uintptr_t allocateFrame(size_t requestSize){


    if (head == nullptr){
        head = initFreeList();
        freeBlock = head;
    }

    bootInformation = getBootInfo();

    size_t freeMemorySize = totalMemory = bootInformation.memory.freeMemSize;
    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    fBlock *selectedBlock = nullptr;

    if (roundedRequestSize > freeMemorySize){
        e9_printf("ERROR!!");
        asm volatile("hlt");
    }
    // best-fit algorithm to search for the appropriate freeBlock
    if (!freeBlock->next && freeBlock->size >= roundedRequestSize) {
        selectedBlock = freeBlock;
    } else {
        // Traverse through the linked list
        while (freeBlock->next) {
            if (freeBlock->size >= roundedRequestSize) {
                if (!selectedBlock || selectedBlock->size > freeBlock->size) {
                    selectedBlock = freeBlock;
                }
            }
            freeBlock = freeBlock->next;
        }
    }
    freeBlock = head;

    if (selectedBlock == nullptr){
        // coalescing setup (merge blocks)
        e9_printf("coalescing setup");
        while(freeBlock->next){
            if (freeBlock->prevSize != 0){
                if ((freeBlock->prevSize + freeBlock->size) >= roundedRequestSize){
                    if (selectedBlock != nullptr){
                        if ((selectedBlock->size + selectedBlock->prevSize) > (freeBlock->size + freeBlock->prevSize)){
                            selectedBlock = freeBlock;
                        }
                    }
                    selectedBlock = freeBlock;
                }
            }
            freeBlock = freeBlock->next;
        }

        freeBlock = head;

        if (selectedBlock == nullptr){
            e9_printf("coalescing halt");
//          implement disk paging
            asm volatile ("hlt");
        }

        // actual coalescing
        fBlock* prevBlock = selectedBlock->previous;
        prevBlock->size += selectedBlock->size;
        prevBlock->next = selectedBlock->next;
//        memoryset(selectedBlock, 0, selectedBlock->size);
        selectedBlock = prevBlock;
    }

    selectedBlock->inUse = true;

    fBlock* newBlock = (fBlock*)((char*)(selectedBlock) + roundedRequestSize);
    newBlock->size = (selectedBlock->size - (roundedRequestSize));
    newBlock->inUse = false;
    newBlock->next = selectedBlock->next;
    newBlock->prevSize = 0;
    head = freeBlock = newBlock;


    bootInformation.memory.freeMemSize = bootInformation.memory.freeMemSize - roundedRequestSize;
    setBootInfo(bootInformation);
    usedMemory += roundedRequestSize;
//    memoryset(selectedBlock, 0, roundedRequestSize);
    insert(&addressSizeHT, (void*)selectedBlock, roundedRequestSize);

    return toPhysicalAddr(selectedBlock);
}
void freeFrame(void* allocatedFrame){
    allocatedFrame = toVirtualAddr(allocatedFrame);
    int allocatedFrameSize = search(&addressSizeHT, (allocatedFrame));
    if (allocatedFrameSize == -1){
        e9_printf("ERROR: Double Free / Wrong Memory Freeing attempt. Blocked.");
        asm volatile("hlt");
    }

    fBlock* freedBlock = (fBlock*)((allocatedFrame));
    freedBlock->size = allocatedFrameSize;
    freedBlock->inUse = false;

    if (freedBlock == head) {
        head = freeBlock = freedBlock;
    } else {
        freedBlock->next = head;
        head->previous = freedBlock;
        head = freedBlock;
    }

    deleteKey(&addressSizeHT, allocatedFrame);
    usedMemory -= allocatedFrameSize;
    if (bootInformation.memory.freeMemSize != 0){
        bootInformation.memory.freeMemSize += allocatedFrameSize;
        setBootInfo(bootInformation);
    }
}