#include "hashtable.hpp"
#include <stdio.h>

//static bootInfo bootInformation;
//
//uintptr_t toPhysicalAddr(void *addr){
//    return (uintptr_t)((uintptr_t)addr - bootInformation.memory.hhdmOffset);
//}
//
//void *toVirtualAddr(void *addr){
//    return (void*)((uintptr_t)addr + bootInformation.memory.hhdmOffset);
//}
//
int main(){
  HashTable ht;
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht.table[i].value = -1; // Initialize all values to -1 (indicating empty)
    }
  insert(&ht, 0x1000, 1);
  insert(&ht, 0x2000, 2);
  
  printf("value for 0x1000 %d\n", search(&ht, 0x1000));
  
  //    bootInformation = getBootInfo();
//  uintptr_t addr = toPhysicalAddr((void*)0x1000);
//    std::cout << addr << std::endl;
//  void* virtualAddr = toVirtualAddr((void*)addr);
//    std::cout << virtualAddr << std::endl;
}
