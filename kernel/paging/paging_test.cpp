#include <iostream>
#include "freeList.hpp"
#include <cstring>
#include <unordered_map>

fBlock* head;
fBlock* block = reinterpret_cast<fBlock*>(malloc(20480000));
fBlock* blockC = block;
size_t totalMemory = 0;
size_t usedMemory = 0;

#define PAGESIZE 4096 // 4KBs

std::unordered_map<void*, size_t> addressSizeHT;

size_t roundUpToPageBoundary(size_t size){
    if ((size % PAGESIZE) == 0){
        return size;
    }

    return static_cast<size_t>(std::ceil(static_cast<double>(size) / PAGESIZE)) * PAGESIZE;
}

void* allocateFrame(size_t requestSize){
    size_t freeMemorySize = totalMemory - usedMemory;
    size_t roundedRequestSize = roundUpToPageBoundary(requestSize);
    fBlock *selectedBlock = nullptr;

    if (roundedRequestSize > freeMemorySize){
        std::cout << "HALT!!!: Not enough memory!!" << std::endl;
        asm volatile("hlt");
    }

    // best-fit algorithm to search for the appropriate block
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

    if (selectedBlock == nullptr){
        std::cout << "inside coalesce setup" << std::endl;
        // coalescing setup (merge blocks)
        while(block->next){
            if (block->prevSize != 0){
                if ((block->prevSize + block->size) >= roundedRequestSize){
                    if (selectedBlock != nullptr){
                        if ((selectedBlock->size + selectedBlock->prevSize) > (block->size + block->prevSize)){
                            selectedBlock = block;
                        }
                    }
                    selectedBlock = block;
                }
            }
            block = block->next;
        }
        std::cout << "stage 1 complete" << std::endl;

        block = head;

        if (selectedBlock == nullptr){
            std::cout << "Unexpected case: Halting!" << std::endl;
            asm volatile ("hlt");
        }

        // actual coalescing
        fBlock* prevBlock = selectedBlock->previous;
        std::cout << "address of prevBlock: " << prevBlock << std::endl;
        std::cout << "address of currentBlock: "  << selectedBlock << std::endl << std::endl;
        std::cout << "Coalesced two blocks of " << prevBlock->size << " and " <<  selectedBlock->size << std::endl;
        prevBlock->size += selectedBlock->size;
        prevBlock->next = selectedBlock->next;
        memset(selectedBlock, 0, selectedBlock->size);
        selectedBlock = prevBlock;
        std::cout << "Coalesced size " << selectedBlock->size << std::endl << std::endl;
    }

    selectedBlock->inUse = true;
    std::cout << "Requested size: " << roundedRequestSize << std::endl;
    std::cout << "Selected size: " << selectedBlock->size << std::endl;

    std::cout << "Starting addr: " << selectedBlock << std::endl << std::endl;
//    std::cout << "Ending addr: " << reinterpret_cast<void*>(reinterpret_cast<char*>(selectedBlock) + roundedRequestSize) << std::endl;

    fBlock* newBlock = reinterpret_cast<fBlock*>(reinterpret_cast<char*>(selectedBlock) + roundedRequestSize);
//    std::cout << "new block: " << newBlock << std::endl;
    newBlock->size = (selectedBlock->size - (roundedRequestSize));
    newBlock->inUse = false;
    newBlock->next = selectedBlock->next;
    newBlock->prevSize = 0;
    std::cout << "head before change: " << head << std::endl;
    head = block = newBlock;
    std::cout << "head after change: " << head << std::endl;
//    std::cout << "New block start: " << newBlock << std::endl;
//    std::cout << "New block size: " << newBlock->size << std::endl;

    usedMemory += roundedRequestSize;
    memset(selectedBlock, 0, roundedRequestSize);
    addressSizeHT.insert({reinterpret_cast<void*>(selectedBlock), roundedRequestSize});

    return selectedBlock;
}

void freeFrame(void* allocatedFrame){
    size_t allocatedFrameSize = addressSizeHT[allocatedFrame];
//    std::cout << "The size of the toBeFreed block is: " << allocatedFrameSize << std::endl;
//    std::cout << "The address of the toBeFreed block is: " << allocatedFrame << std::endl;

    memset(allocatedFrame, 0, allocatedFrameSize);

    fBlock* freedBlock = reinterpret_cast<fBlock*>(allocatedFrame);
    freedBlock->size = allocatedFrameSize;
    freedBlock->inUse = false;

    head->prevSize = freedBlock->size;
    head->previous = freedBlock;
    freedBlock->next = head;
    head = block = freedBlock;
    addressSizeHT.erase(allocatedFrame);
    usedMemory -= allocatedFrameSize;


}

void printBlocks() {
    fBlock* temp = head;
    size_t accumulatedPrevSize = 0;

    std::cout << "---" << std::endl;
    std::cout << std::endl << "Free List Info: " << std::endl;

    while (temp) {
        std::cout << "address of the current node: " << temp << std::endl;
        std::cout << "address of the next node: " << temp->next << std::endl << "address of the prev node: " << temp->previous << std::endl;
        std::cout << "size of the prev node: " << temp->prevSize << std::endl;
        std::cout << "-----" << std::endl << std::endl;
//        std::cout << "address of the prev node: "
//                  << reinterpret_cast<void*>(reinterpret_cast<char*>(temp) - accumulatedPrevSize) << std::endl;

        temp = temp->next;
    }

    std::cout << "-----" << std::endl << std::endl;
}

int main(){
    block->size = 20480;
    block->inUse = false;
    block->prevSize = 0;
    block->next = nullptr;
    block->previous = nullptr;
    head = block;
//  this setup until here is equivalent to calling initFreeList
    totalMemory = block->size;

    std::cout << "Initial block size: " << block->size << std::endl;
    void* page = allocateFrame(1000);
    void* page2 = allocateFrame(1000);
    void* page3 = allocateFrame(1000);
    void* page4 = allocateFrame(1000);
    std::cout << "Final block size: " << block->size << std::endl << std::endl;
    std::cout << "Used Memory: " << usedMemory << std::endl;
    std::cout << "Free Memory: " << totalMemory - usedMemory << std::endl << std::endl;
    freeFrame(page);
    freeFrame(page2);
    printBlocks();
    std::cout << std::endl << "Free Memory after two frees: " << totalMemory - usedMemory << std::endl << std::endl;
    void* page5 = allocateFrame(8192);
    std::cout << std::endl << "Free Memory after coalescing: " << totalMemory - usedMemory << std::endl << std::endl;
    printBlocks();
    free(blockC);

    return 0;
}