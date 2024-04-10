#include <cstdint>
#include "malloc.hpp"
#include "../lib/string.hpp"
#include "../boot.h"
#include "../paging/pageFrameAllocator.hpp"

head* freeList = nullptr;
uint32_t mutex = 0;

// to-do: create a way to use user-mode pages for malloc
void initNodes(){
    bootInfo bootInformation = getBootInfo();
    void* memory = toVirtualAddr((void*)allocateFrame((12*1024*1024) + (sizeof(head) * POOL_ARRAY_SIZE)));

    freeList = (head*)memory;
    head* currentNode = freeList;

    for (int i = 0; i < (POOL_ARRAY_SIZE - 1); i++) {
        if (i != 0) {
            currentNode->previous = (head*)((uintptr_t)currentNode - PAGE_SIZE);
        }
        else {
            currentNode->previous = nullptr;
        }

        currentNode->next = (head*)((uintptr_t)currentNode + PAGE_SIZE);
        currentNode->left = currentNode->right = nullptr;
        currentNode->status = unused;
        currentNode->size = PAGE_SIZE;
        currentNode->childSize = currentNode->size / 2;
        currentNode = currentNode->next;
    }
}

void* mallocx(size_t size) {
    lock(mutex);

    if (freeList == nullptr) {
        initNodes();
    }

    size = size + sizeof(head);

    if (size < MINIMUM_ALLOCATION_SIZE) {
        size = MINIMUM_ALLOCATION_SIZE;
    }

    size = roundUpToNextPower2(size);

    head* currentNode = freeList;
    while (true) {
        if (size > PAGE_SIZE) {
            if ((currentNode->level == 0) && (currentNode->status != allocated) && (currentNode->next != nullptr)) {
                size_t blocksToCoalesce = size / PAGE_SIZE;
                head* tempNode{};
                bool canCoalesce = true;

                if (blocksToCoalesce > MAXIMUM_COALESCE_BLOCKS) {
                    unlock(mutex);
                    return nullptr;
                }

                tempNode = currentNode->next;
                for (size_t i = 0; i < blocksToCoalesce - 1; i++) {
                    if (tempNode->status == allocated) {
                        canCoalesce = false;
                        break;
                    }
                    tempNode = tempNode->next;
                }

                if (canCoalesce) {
                    unlock(mutex);
                    return coalesceBlocks(currentNode, blocksToCoalesce);
                }
            }
        }

        if ((currentNode->left != nullptr) && (currentNode->left->status == allocated)) {
            if ((currentNode->right != nullptr)){
                if ((currentNode->right->status == unused)) {
                    currentNode = currentNode->right;
                }
                else {
                    currentNode = currentNode->next;
                }
            }
        }
        else if (currentNode->status == allocated) {
            if (currentNode->next != nullptr) {
                currentNode = currentNode->next;
            }
        }

        if (currentNode->size == size) {
            unlock(mutex);
            break;
        }

        if ((currentNode->status == allocated) || (size > currentNode->size)){
            if (currentNode->next != nullptr) {
                currentNode = currentNode->next;
            }
            continue;
        }

        currentNode->left = currentNode;
        currentNode->right = (head*)((uintptr_t)currentNode + currentNode->childSize);
        currentNode->status = unused;
        currentNode->size = currentNode->size / 2;
        currentNode->right->size = currentNode->size;
        currentNode->right->next = currentNode->next;
        currentNode = currentNode->left;
    }

    currentNode->status = allocated;

    unlock(mutex);

    return (void*)((uintptr_t)currentNode + sizeof(head));
}

void* coalesceBlocks(head* node, size_t n) {
    lock(mutex);
    head* lastNode{};

    lastNode = node;
    for (size_t i = 0; i < n - 1; i ++) {
        lastNode = lastNode->next;
    }

    node->next = lastNode->next;
    node->size = PAGE_SIZE * n;
    node->status = allocated;

    unlock(mutex);
    return (void*)((uintptr_t)node + sizeof(head));
}


void freex(void* block) {
    lock(mutex);
    head* node = (head*)((uintptr_t)block - sizeof(head));
    node->status = unused;
    int usuableSize = ((uintptr_t)node->next - (uintptr_t)node - (sizeof(head) * 2));

    if (node->isBuddyCoalesced) {
        head* buddy = (head*)((uintptr_t)node ^ node->size);
        node->size = buddy->size;
        node->status = buddy->status = unused;
        node->isBuddyCoalesced = false;
        buddy->next = node->next;
        unlock(mutex);
        return;
    }

    if (node->size > PAGE_SIZE) {
        size_t coalescedBlocks = node->size / PAGE_SIZE;
        head* tempNode = node;
        coalescedBlocks--;
        while (coalescedBlocks) {
            head* nextPointer = (head*)((uintptr_t)tempNode + PAGE_SIZE);

            nextPointer->previous = tempNode;
            nextPointer->size = PAGE_SIZE;
            nextPointer->level = 0;
            nextPointer->status = unused;
            nextPointer->childSize = nextPointer->size / 2;
            nextPointer->next = (head*)((uintptr_t)nextPointer + PAGE_SIZE);
            tempNode->next = nextPointer;
            tempNode->size = PAGE_SIZE;
            tempNode = nextPointer;
            coalescedBlocks--;
        }
    }
    else {
//        memoryset(block, 0, usuableSize);
    }
    unlock(mutex);
}

void* reallocx(void *block, size_t size) {
    // increase the size of the current block
    // if the next block to which we want to expand is already allocated
    // copy all the data of this block and allocate a block of size and return it
    lock(mutex);
    size_t roundedSize = roundUpToNextPower2(size);

    head* node = (head*)((uintptr_t)block - sizeof(head));
    head* buddyNode = (head*)((uintptr_t)node ^ node->size);

    if (roundedSize == node->size) {
        unlock(mutex);
        return block;
    }

    if (roundedSize >= node->size * 2 || buddyNode->status == allocated) {
        void* newNode = (head*)(mallocx(size));
        if (newNode != nullptr) {
            memorycpy(newNode, block, node->size - sizeof(head));
            head* newNodeData = (head*)((uintptr_t)newNode - sizeof(head));
            newNodeData->level = node->level;
            newNodeData->status = node->status;
            newNodeData->size = roundedSize;
            freex(block);
            unlock(mutex);
            return (void*)newNode;
        }
    }

    buddyNode->status = allocated;
    node->isBuddyCoalesced = true;
    node->size = node->size * 2;
    unlock(mutex);
    return (void*)((uintptr_t)node - sizeof(head));
}

constexpr size_t roundUpToNextPower2(size_t size) {
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;
    return size;
}