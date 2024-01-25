#include "kernelEntry.hpp"
#include "boot.h"
#include "gdt/gdt.hpp"
#include "idt/idt.hpp"
#include "idt/isr.hpp"
#include "printutils/e9print.h"
#include "paging/pageFrameAllocator.hpp"
#include "paging/paging.hpp"

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
    GlobalRenderer->Print(" bytes\n");
    GlobalRenderer->Print("bootinfo: ");
    GlobalRenderer->PrintInt(bootInformation.memory.hhdmOffset);

    char* frame = (char*)allocateFrame(1000);
    char* frame1 = (char*)allocateFrame(1000);
//    frame = "bruh";
//    GlobalRenderer->Print(frame);
    bootInformation = getBootInfo();
    GlobalRenderer->Print("\nMemory size after allocation: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    freeFrame(frame);
    freeFrame(frame1);
    GlobalRenderer->Print("\nFreed stuff");
//    initPaging();
    return 0;
}
