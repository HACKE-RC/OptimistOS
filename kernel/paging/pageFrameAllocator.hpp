#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../boot.h"
#include "../kernelEntry.hpp"
#include "../printutils/e9print.h"
#include "../renderer/BasicRenderer.hpp"
#include "freeList.hpp"

void* allocateFrame(size_t requestSize);
void freeFrame(void* frameAddr);

