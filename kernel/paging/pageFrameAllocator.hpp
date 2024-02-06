#pragma once
#ifndef pageframeallocator
#define pageframeallocator

#include "../boot.h"
#include "../kernelEntry.hpp"
#include "../printutils/e9print.h"
#include "../renderer/BasicRenderer.hpp"
#include "freeList.hpp"
#include "../lib/string.hpp"

#include <cstdint>
#include <cstddef>
#include <cstddef>
#include "hashtable.hpp"
#include <cassert>



static HashTable addressSizeHT;
static bool initialised = false;
void freeFrame(void* allocatedFrame);
//void* allocateFrame(size_t requestSize);
size_t roundUpToPageBoundary(size_t size);
uint64_t roundUp(uint64_t n, uint64_t toAlign);
uint64_t roundDown(uint64_t n, uint64_t toAlign);
uintptr_t allocateFrame(size_t requestSize);
void freeFrame(void* frameAddr);
void *toVirtualAddr(void *addr);
uintptr_t toPhysicalAddr(void *addr);

#endif