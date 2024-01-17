#include <iostream>
#include "freeList.hpp"

fBlock* block = new fBlock;
fBlock *freeBlock = block;
#define PAGESIZE 4096 // 4KBs

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }

    return static_cast<size_t>(std::ceil(static_cast<double>(size) / PAGESIZE)) * PAGESIZE;
}

void* allocateFrame(size_t requestSize){
    if (requestSize > freeBlock->size){
        std::cout << "ERROR" << std::endl;
        asm volatile("hlt");
    }
    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    // best-fit algorithm to search for the appropriate block
    fBlock *selectedBlock = new fBlock;
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

    selectedBlock->inUse = true;

}

int main(){
    block->size = 20480;
    block->inUse = false;
    block->prevSize = 0;
    block->next = nullptr;

    allocateFrame(1000);
    std::cout << block->size << std::endl;
    return 0;
}