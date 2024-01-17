#include <iostream>
#include "freeList.hpp"

fBlock* head;
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
    fBlock *selectedBlock = nullptr;

    if (!block->next && block->size >= roundedRequestSize) {
        selectedBlock = block;
    } else {
        // Traverse through the linked list
        while (block->next) {
            if (block->size >= roundedRequestSize) {
                if (!selectedBlock || selectedBlock->size > block->size) {
                    selectedBlock = block;
                }
            }
            block = block->next;
        }
    }
    block = head;

    selectedBlock->inUse = true;
    std::cout << "Requested size: " << roundedRequestSize << std::endl;
    std::cout << "Selected size: " << selectedBlock->size << std::endl;

    std::cout << "Starting addr: " << selectedBlock << std::endl;
    std::cout << "Ending addr: " << reinterpret_cast<void*>(reinterpret_cast<char*>(selectedBlock) + roundedRequestSize) << std::endl;

    fBlock* newBlock = reinterpret_cast<fBlock*>(reinterpret_cast<char*>(selectedBlock) + roundedRequestSize);
    newBlock->size = (selectedBlock->size - (roundedRequestSize + sizeof(struct fBlock)));
    std::cout << "New block addr: " << newBlock << std::endl;
}

int main(){
    block->size = 20480;
    block->inUse = false;
    block->prevSize = 0;
    block->next = nullptr;

    head=block;

    std::cout << "Initial block size: " << block->size << std::endl;
    allocateFrame(1000);
    std::cout << "Final block size: " << block->size << std::endl;
    return 0;
}