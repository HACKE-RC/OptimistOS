#include "pageFrameAllocator.hpp"
size_t totalMemory = 0;
size_t usedMemory = 0;

void memoryset(void *start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
    {
        *(uint8_t *)((uint64_t)start + i) = value;
    }

}

fBlock *head = initFreeList();
fBlock *freeBlock = head;
bootInfo bootInformation = getBootInfo();

#define PAGESIZE 4096 // 4KBs

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }
    return (size + PAGESIZE - 1) & ~(PAGESIZE - 1);
//    return static_cast<size_t>(std::ceil((size) / PAGESIZE)) * PAGESIZE;
}

void* allocateFrame(size_t requestSize){
    initHashMap(&addressSizeHT);
    size_t freeMemorySize = totalMemory - usedMemory;
    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    fBlock *selectedBlock = nullptr;

    if (roundedRequestSize > freeMemorySize){
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
            asm volatile ("hlt");
        }

        // actual coalescing
        fBlock* prevBlock = selectedBlock->previous;
        prevBlock->size += selectedBlock->size;
        prevBlock->next = selectedBlock->next;
        memoryset(selectedBlock, 0, selectedBlock->size);
        selectedBlock = prevBlock;
    }

    selectedBlock->inUse = true;

    fBlock* newBlock = (fBlock*)((char*)(selectedBlock) + roundedRequestSize);
    newBlock->size = (selectedBlock->size - (roundedRequestSize));
    newBlock->inUse = false;
    newBlock->next = selectedBlock->next;
    newBlock->prevSize = 0;
    head = freeBlock = newBlock;

    usedMemory += roundedRequestSize;
    memoryset(selectedBlock, 0, roundedRequestSize);
//    addressSizeHT.insert({reinterpret_cast<void*>(selectedBlock), roundedRequestSize});
    insert(&addressSizeHT, (void*)selectedBlock, roundedRequestSize);
    return selectedBlock;
}

void freeFrame(void* allocatedFrame){
    size_t allocatedFrameSize = findKeyByHash(&addressSizeHT, allocatedFrame);
//    size_t allocatedFrameSize = addressSizeHT[allocatedFrame];
//    size_t allocatedFrameSize = 123;
    memoryset(allocatedFrame, 0, allocatedFrameSize);

    fBlock* freedBlock = (fBlock*)(allocatedFrame);
    freedBlock->size = allocatedFrameSize;
    freedBlock->inUse = false;

    head->prevSize = freedBlock->size;
    head->previous = freedBlock;
    freedBlock->next = head;
    head = freeBlock = freedBlock;
    removeEntry(&addressSizeHT, allocatedFrame);
//    addressSizeHT.erase(allocatedFrame);
    usedMemory -= allocatedFrameSize;
}

//void printBlocks() {
//    fBlock* temp = head;
//    size_t accumulatedPrevSize = 0;
//
//
//    while (temp) {
//        std::cout << "address of the current node: " << temp << std::endl;
//        std::cout << "address of the next node: " << temp->next << std::endl << "address of the prev node: " << temp->previous << std::endl;
//        std::cout << "size of the prev node: " << temp->prevSize << std::endl;
//        std::cout << "-----" << std::endl << std::endl;
////        std::cout << "address of the prev node: "
////                  << reinterpret_cast<void*>(reinterpret_cast<char*>(temp) - accumulatedPrevSize) << std::endl;
//
//        temp = temp->next;
//    }
//
//    std::cout << "-----" << std::endl << std::endl;
//}