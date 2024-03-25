#ifndef MALLOCX_MALLOC_HPP
#define MALLOCX_MALLOC_HPP

// source: https://github.com/HACKE-RC/mallocx

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include "../lib/lock.hpp"

#define MIN 5
#define LEVELS 7
#define PAGE_SIZE 4096
#define POOL_SIZE 0xC00000
#define POOL_ARRAY_SIZE 3072
#define MINIMUM_ALLOCATION_SIZE 32
#define MAXIMUM_COALESCE_BLOCKS 4

enum flag{
    unused,
    allocated
};

struct head{
    head* previous = nullptr;
    head* right = nullptr;
    head* left = nullptr;
    head* next = nullptr;
    flag status = unused;
    size_t childSize = 0;
    size_t size = 0;
    short level = 0;
    bool isBuddyCoalesced = false;
};

extern void initNodes();
extern void freex(void* block);
extern void* mallocx(size_t size);
extern void* reallocx(void* block, size_t size);
extern void* coalesceBlocks(head* node, size_t n);
constexpr size_t roundUpToNextPower2(size_t size);
#endif