#include <iostream>
#include "freeList.hpp"
#include <cstring>
#include <unordered_map>

fBlock* head;
fBlock* block = new fBlock;
#define PAGESIZE 4096 // 4KBs

std::unordered_map<void*, size_t> addressSizeHT;

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
    newBlock->size = (selectedBlock->size - (roundedRequestSize));
    newBlock->inUse = false;
    newBlock->next = nullptr;
    newBlock->prevSize = 0;

    head = block = newBlock;
    std::cout << "New block start: " << newBlock << std::endl;
    std::cout << "New block size: " << newBlock << std::endl;
    memset(selectedBlock, 0, roundedRequestSize);
    addressSizeHT.insert({reinterpret_cast<void*>(selectedBlock), roundedRequestSize});
    return selectedBlock;
}

void freeFrame(void* allocatedFrame){
    size_t allocatedFrameSize = addressSizeHT[allocatedFrame];
    std::cout << "The size of the toBeFreed block is: " << allocatedFrameSize << std::endl;
    std::cout << "The address of the toBeFreed block is: " << allocatedFrame << std::endl;
    memset(allocatedFrame, 0, allocatedFrameSize);
    fBlock* freedBlock = reinterpret_cast<fBlock*>(allocatedFrame);
    freedBlock->size = allocatedFrameSize;
    freedBlock->inUse = false;

    head->prevSize = freedBlock->size;
    freedBlock->next = head;
    head = block = freedBlock;
    addressSizeHT.erase(allocatedFrame);
    std::cout << "address of the new head: " << head << std::endl;
    fBlock *temp = head;
    while(temp->next) {
        std::cout << "address of the next node: " << head << std::endl;
        temp = temp->next;
    }
}

int main(){
    block->size = 20480;
    block->inUse = false;
    block->prevSize = 0;
    block->next = nullptr;
    head = block;
//  this setup until here is equivalent to calling initFreeList

    std::cout << "Initial block size: " << block->size << std::endl;
    std::cout << "Initial block address: " << block << std::endl;
    void* page = allocateFrame(1000);
    std::cout << "Allocated page addr: " << page << std::endl;
    strcpy((char*)page, "Hello!");
    std::cout << (char*)page << std::endl;
    std::cout << "Final block size: " << block->size << std::endl;
    freeFrame(page);
    return 0;
}