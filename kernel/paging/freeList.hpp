#pragma once
#ifndef freelist
#define freelist

#include "../boot.h"
#include "../printutils/e9print.h"
#include "../renderer/BasicRenderer.hpp"
#include "pageFrameAllocator.hpp"
#include <cmath>
#include <stdint.h>
#include <stddef.h>

struct fBlock{
    size_t size;
    fBlock *next;
    fBlock *previous;
    bool inUse;
    size_t prevSize;
};

fBlock* initFreeList();


#endif
