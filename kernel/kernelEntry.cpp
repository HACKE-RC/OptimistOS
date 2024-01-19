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
    GlobalRenderer->Print(" bytes");

    void* frame = allocateFrame(214000800);
    bootInformation = getBootInfo();
    GlobalRenderer->Print("\nMemory size after allocation: ");
    GlobalRenderer->PrintInt(bootInformation.memory.freeMemSize);
    freeFrame(frame);
    return 0;
}
