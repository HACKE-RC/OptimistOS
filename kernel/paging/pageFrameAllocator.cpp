#include "pageFrameAllocator.hpp"

fBlock *freeBlock = initFreeList();
bootInfo bootInformation = getBootInfo();

void* allocateFrame(size_t requestSize){
    if (requestSize > freeBlock->size){
        e9_print("ERR!\n");
        e9_print("Not enough memory to allocate!");
        asm volatile("hlt");
    }


}

