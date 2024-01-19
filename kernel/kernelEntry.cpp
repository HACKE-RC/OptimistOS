#include "kernelEntry.hpp"
#include "boot.h"
#include "gdt/gdt.hpp"
#include "idt/idt.hpp"
#include "idt/isr.hpp"
#include "printutils/e9print.h"
#include "paging/pageFrameAllocator.hpp"

static BasicRenderer renderer = BasicRenderer(NULL, NULL);

int setupOptimist(){
    bootInfo bootInformation = getBootInfo();
    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Clear(Colors.black, true);
    GlobalRenderer->Print("this is working!\n");
    initGDT();
    isrInstall(renderer);
    GlobalRenderer->Print("Memory Information: \n");
    GlobalRenderer->Print("Old Memory Size: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    GlobalRenderer->Print(" B");

    void* frame = allocateFrame(214000800);
    for (int i = 0; i < 9; i++){
        frame = allocateFrame(214000800);
        bootInformation = getBootInfo();
        GlobalRenderer->Print("\nMemory size after allocation: ");
        GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    }
    for (int i = 0; i < 10; i++){
        frame = allocateFrame(100000);
        bootInformation = getBootInfo();
        GlobalRenderer->Print("\nMemory size after allocation: ");
        GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    }
    for (int i = 0; i < 5; i++){
        frame = allocateFrame(1000);
        GlobalRenderer->Print("\nMemory size after allocation: ");
        GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
        bootInformation = getBootInfo();
    }
    bootInformation = getBootInfo();
    GlobalRenderer->Print("\nMemory size after allocation: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
//
//
//    void* frame1 = allocateFrame(1000);

    GlobalRenderer->Print("\nFree 4096\n");
    freeFrame(frame);
    bootInformation = getBootInfo();
    GlobalRenderer->Print("\nMemory size after free: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
//    frame = allocateFrame(1000);
//
//    bootInformation = getBootInfo();
//    GlobalRenderer->Print("\nMemory size after : ");
//    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
//    freeFrame(frame);
    frame = allocateFrame(8192);

    bootInformation = getBootInfo();
    GlobalRenderer->Print("\nMemory size after coalescing: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    return 0;
}
