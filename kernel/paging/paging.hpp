#ifndef paging 
#define paging 
#define _1GB 0x40000000ul
#include "../boot.h"
#include "pageFrameAllocator.hpp"
#include <stdint.h>

extern "C" void setCr3(void*);

enum pageTableFlag{
    Present = (1 << 0),
    ReadWrite = (1 << 1),
    UserOrSuperuser = (1 << 2),
    WriteThrough = (1 << 3), // PWT
    CacheDisable = (1 << 4), // PCD
    Accessed = (1 << 5),
    LargerPages = (1 << 7),
    PAT4k = (1 << 7), // PAT lvl1
    Global = (1 << 8),
    Custom0 = (1 << 9),
    Custom1 = (1 << 10),
    Custom2 = (1 << 11),
    PATlg = (1 << 12), // PAT lvl2+
    NoExec = (1UL << 63)
};

struct PageDirectoryEntry{
  uint64_t value;

  void setFlag(pageTableFlag flag, bool enable){
        uint64_t bitSelector = (uint64_t) 1 << flag;
        value &= ~bitSelector;

        // request to set the flag
        if (enable){
            value |= bitSelector;
        }
  }

  bool getFlag(pageTableFlag flag){
      uint64_t bitSelector = (uint64_t) 1 << flag;
      return ((value & bitSelector) > 0);
  }

void setAddress(uint64_t address){
    address &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff; // Clear address bits
    value |= (address << 12); // Set new address
    value |= (value & 0xf000000000000fff); // Set flags back
}

  uint64_t getAddress(){
      return (value & 0x000ffffffffff000) >> 12;
  }

  bool isValid(){
      if (getAddress()!=0){
          return true;
      }
      return false;
  }
} __attribute__((packed));

struct PageTable{
    PageDirectoryEntry entries[512];
};

void initPaging();
bool map(uintptr_t physicalAddr, void* virtualAddr, pageTableFlag flags);
void setCr3(uint64_t value);
uintptr_t getNextLevelPointer(PageDirectoryEntry& entry, bool allocate);
PageDirectoryEntry *virtualAddrToPTE(void* virtualAddr, bool allocate, pageTableFlag flags);
uint64_t readCr3();
#endif
