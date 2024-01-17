#include <iostream>
#include "freeList.hpp"

fBlock* block = new fBlock;
//fBlock *block= block;
#define PAGESIZE 4096 // 4KBs

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }

    return static_cast<size_t>(std::ceil(static_cast<double>(size) / PAGESIZE)) * PAGESIZE;
}

void* allocateFrame(size_t requestSize){
    if (requestSize > block->size){
        std::cout << "ERROR" << std::endl;
        asm volatile("hlt");
    }

    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    // best-fit algorithm to search for the appropriate block
    fBlock *selectedBlock = new fBlock;
    selectedBlock->size = 0;

    do{
        if ((block->size >= roundedRequestSize)){
            if (selectedBlock->size){
                if (selectedBlock->size > block->size){
                    selectedBlock = block;
                }
            }
            else{
                selectedBlock = block;
            }
        }
    } while(block->next);

    selectedBlock->inUse = true;

    std::cout << "Inside func: " << block->size << std::endl;
    std::cout << "Starting addr: " << (&selectedBlock) << std::endl;
    std::cout << "Ending addr: " << (&selectedBlock + sizeof(struct fBlock)) << std::endl;
}

int main(){
    block->size = 20480;
    block->inUse = false;
    block->prevSize = 0;
    block->next = nullptr;

    std::cout << block->size << std::endl;
    allocateFrame(8200);
    std::cout << block->size << std::endl;
    return 0;
}