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
#include "hashtable.hpp"

static void memoryset(void *start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
    {
        *(uint8_t *)((uint64_t)start + i) = value;
    }

}

static HashTable addressSizeHT;
static bool initialised = false;
void freeFrame(void* allocatedFrame);
//void* allocateFrame(size_t requestSize);
size_t roundUpToPageBoundary(size_t size);

uintptr_t allocateFrame(size_t requestSize);
void freeFrame(void* frameAddr);
void *toVirtualAddr(void *addr);
uintptr_t toPhysicalAddr(void *addr);

#endif