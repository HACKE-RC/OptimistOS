#pragma once
#ifndef freelist
#define freelist

#include "paging.hpp"
#include "../boot.h"
#include "pageFrameAllocator.hpp"
#include "../printutils/e9print.h"
#include "../renderer/BasicRenderer.hpp"

#include <cstdint>
#include <cstddef>

struct fBlock{
    size_t size;
    fBlock *next;
    fBlock *previous;
    bool inUse;
    size_t prevSize;
};

fBlock* initFreeList();


#endif
