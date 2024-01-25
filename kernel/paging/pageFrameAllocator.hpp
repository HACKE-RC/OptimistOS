#pragma once
#ifndef pageframeallocator
#define pageframeallocator

#include <stdint.h>
#include <stddef.h>
#include "../boot.h"
#include "../kernelEntry.hpp"
#include "../printutils/e9print.h"
#include "../renderer/BasicRenderer.hpp"
#include "freeList.hpp"
#include <stddef.h>
#include <stdbool.h>

#define TABLE_SIZE 1024
#define MAX_NODES (TABLE_SIZE * 2)

typedef struct Node {
    void* hash;
    size_t key;
    struct Node* next;
} Node;

typedef struct {
    Node nodes[MAX_NODES];
    Node* heads[TABLE_SIZE];
    size_t numNodes;
} HashMap;

static void initHashMap(HashMap* map) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        map->heads[i] = &map->nodes[i];
    }
    map->numNodes = TABLE_SIZE;
}

static void insert(HashMap* map, void* hash, size_t key) {
    size_t index = (size_t)hash % TABLE_SIZE;

    Node* curr = map->heads[index];
    while(curr->next != NULL) {
        curr = curr->next;
    }

    if (map->numNodes == MAX_NODES) {
        // handle overflow
        return;
    }

    int i;
    for(i = 0; i < MAX_NODES; i++) {
        if(map->nodes[i].hash == NULL) {
            // found unused node
            break;
        }
    }

    curr->next = &map->nodes[i];
    curr->next->hash = hash;
    curr->next->key = key;
    curr->next->next = NULL;
    map->numNodes++;
}

static int findKeyByHash(HashMap* map, void* hash) {
    int index = (size_t)hash % TABLE_SIZE;

    Node* current = map->heads[index];
    while(current != NULL) {
        if(current->hash == hash) {
            return current->key;
        }
        current = current->next;
    }

    return -1; // value not found
}
static void removeEntry(HashMap* map, void* hash) {
    size_t index = (size_t)hash % TABLE_SIZE;

    Node *curr = map->heads[index];
    Node *prev = NULL;

    while(curr != NULL) {
        if (curr->hash == hash) {
            if (prev == NULL) {
                // removing head
                map->heads[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            curr->hash = NULL;
            curr->next = NULL;
            map->numNodes--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

static void memoryset(void *start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
    {
        *(uint8_t *)((uint64_t)start + i) = value;
    }

}

static HashMap addressSizeHT;
void freeFrame(void* allocatedFrame);
//void* allocateFrame(size_t requestSize);
size_t roundUpToPageBoundary(size_t size);

uintptr_t allocateFrame(size_t requestSize);
void freeFrame(void* frameAddr);
void *toVirtualAddr(void *addr);
uintptr_t toPhysicalAddr(void *addr);

#endif