#include "pageFrameAllocator.hpp"

fBlock *freeBlock = initFreeList();
bootInfo bootInformation = getBootInfo();

#define PAGESIZE 4096 // 4KBs

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }

    return static_cast<size_t>(std::ceil(static_cast<double>(size) / PAGESIZE)) * PAGESIZE;
}

void* allocateFrame(size_t requestSize){
    if (requestSize > freeBlock->size){
        e9_print("ERR!\n");
        e9_print("Not enough memory to allocate!");
        asm volatile("hlt");
    }
    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    // best-fit algorithm to search for the appropriate block
    fBlock *selectedBlock;
    selectedBlock->size = 0;

    do{
        if ((freeBlock->size >= roundedRequestSize)){
            if (selectedBlock->size){
                if (selectedBlock->size > freeBlock->size){
                    selectedBlock = freeBlock;
                }
            }
            else{
                selectedBlock = freeBlock;
            }
        }
    } while(freeBlock->next);



}

