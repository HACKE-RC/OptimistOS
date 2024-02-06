#ifndef paging 
#define paging 

#define _1GB 0x40000000ul
#define _4KB 0x1000

#include "../boot.h"
#include "pageFrameAllocator.hpp"
#include <utility>
#include <cstdint>

#define writeCrReg(reg, value)			\
({							\
    asm volatile("mov %0, %%cr" #reg : : "r" (value));	\
})

enum controlRegs
{
    cr0 = 0000000,
    cr1 = 0001000,
    cr2 = 0002000,
    cr3 = 0003000,
};
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
  uint64_t value = 0;

  void setFlag(pageTableFlag flag, bool enable){
        value &= ~flag;

        // request to set the flag
        if (enable){
            value |= flag;
        }
  }

  [[nodiscard]] bool getFlag(pageTableFlag flag) const{
      return ((value & flag));
  }

void setAddress(uint64_t address){
    value &= ~0x000FFFFFFFFFF000;
    value |= (address & 0x000FFFFFFFFFF000);
  }

  [[nodiscard]] uint64_t getAddress() const{
      if (value!=0){
          return value & 0x000FFFFFFFFFF000;
      }
      else{
          return 0;
      }
  }

  [[nodiscard]] bool isValid() const{
      if (getFlag(Present)){
          if (getAddress() == (uintptr_t)nullptr){
              return false;
          }
          return true;
      }
      return false;
  }
} __attribute__((packed));

struct PageTable{
    PageDirectoryEntry entries[512];
};

extern void initPaging();
extern uint64_t readCr3();
extern bool isHigherHalf(uintptr_t addr);
extern bool map(uintptr_t physicalAddr, void* virtualAddr, pageTableFlag flags, size_t pageSize = _4KB);
extern PageDirectoryEntry *virtualAddrToPTE(void* virtualAddr, bool allocate, pageTableFlag flags, size_t pageSize = _4KB);
extern uintptr_t getNextLevelPointer(PageDirectoryEntry& entry, bool allocate, void* virtualAddr= nullptr, size_t pageSize = _4KB);
extern std::pair<size_t, size_t> requiredSize(size_t size);
extern PageTable* PML4;

#endif
