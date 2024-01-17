#ifndef OPTIMISTOS_FREELIST_HPP
#define OPTIMISTOS_FREELIST_HPP
//#include "../boot.h"
//#include "../printutils/e9print.h"
//#include "../renderer/BasicRenderer.hpp"
//#include "pageFrameAllocator.hpp"
#include <iostream>
#include <cmath>
#include <stdint.h>
#include <stddef.h>

struct fBlock{
    size_t size;
    fBlock *next;
    bool inUse;
    size_t prevSize;
};

fBlock* initFreeList();
void addNode(size_t size);

#endif //OPTIMISTOS_FREELIST_HPP
