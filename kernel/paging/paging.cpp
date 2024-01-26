#include "paging.hpp"
bootInfo bootInformation{};
//void setCR3(void* address) {
//    uintptr_t physAddr = reinterpret_cast<uintptr_t>(address);
//    asm volatile ("mov %0, %%cr3" :: "r" (physAddr) : "%cr3");
//}
uint64_t readCr3();

void setCr3(uint64_t value) {
    asm volatile("movq %0, %%cr3" : : "r" (value));
}

void initPaging(){
    bootInformation = getBootInfo();
    auto* PML4 = (struct PageTable*)allocateFrame(4096);
    auto cr3 = (uintptr_t)PML4;
    e9_printf("\nPhysical Address of PML4 #2: %x", PML4);
    e9_printf("\nLast call!");
    PML4 = (struct PageTable*)toVirtualAddr(PML4);
    unsigned long long entry;
    for (auto & pageDirectoryEntry : PML4->entries){
        pageDirectoryEntry.setFlag(UserOrSuperuser, true);
        pageDirectoryEntry.setFlag(ReadWrite, true);
        pageDirectoryEntry.setFlag(Present, false);
        entry = pageDirectoryEntry.value;
    }
    e9_printf("\nentry value: %x\n", entry);
//    setCr3(cr3);
//    e9_printf("\nPhysical Address of PML4 #1: %x", virtual_to_physical(PML4));
//    e9_printf("\nHHDM offset: %x", bootInformation.memory.hhdmOffset);
    e9_printf("\nLast call 2!");
    e9_printf("\nPML4 setup complete");
    e9_printf("\nreg value: %x", readCr3());
}

uint64_t readCr3() {
    uint64_t cr3Value;
    __asm__ volatile ("mov %%cr3, %0" : "=r" (cr3Value));
    return cr3Value;
}

uintptr_t virtual_to_physical(void* virtual_address) {
    uintptr_t addr = (uintptr_t)virtual_address;

    // Extract the indices and offset
    uintptr_t offset = addr & 0xFFF;
    uintptr_t pdpt_index = (addr >> 12) & 0x1FF;
    uintptr_t pt_index = (addr >> 21) & 0x1FF;

    // Get the addresses of the PML4, PDPT, and PT
    uintptr_t pml4_base = cpuGetControlReg(3) & ~0xFFF;
    uintptr_t pdpt_base = *(uintptr_t*)(pml4_base + (pdpt_index << 3)) & ~0xFFF;
    uintptr_t pt_base = *(uintptr_t*)(pdpt_base + (pt_index << 3)) & ~0xFFF;

    // Get the physical frame number
    uintptr_t frame_number = *(uintptr_t*)(pt_base + (offset << 3));

    // Calculate the physical address
    uintptr_t physical_address = frame_number + offset;

    return physical_address;
}
uintptr_t cpuGetControlReg(uint32_t reg) {
    uintptr_t val;
    switch (reg) {
        case 3: // CR3
            __asm__ volatile("mov %%cr3, %0" : "=r"(val));
            break;
            // Add cases for other control registers as needed
    }
    return val;
}
