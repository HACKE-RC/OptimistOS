#include "kernelEntry.hpp"
#include "boot.h"
#include "gdt/gdt.hpp"
#include "idt/idt.hpp"
#include "idt/isr.hpp"
#include "printutils/e9print.h"

static BasicRenderer renderer = BasicRenderer(NULL, NULL);

int setupOptimist(bootInfo bootInformation){
    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Clear(Colors.black, true);
    GlobalRenderer->Print("this is working!\n");
    initGDT();
    isrInstall(renderer);
    GlobalRenderer->Print("Memory Information: \n");
    GlobalRenderer->Print("Free Memory Location: ");
    e9_printf("%d\n", bootInformation.memory.freeMemStart);
    GlobalRenderer->Print("Free Memory Size: ");
    e9_printf("%d\n", bootInformation.memory.freeMemSize);
    GlobalRenderer->Print("gdt is done!\n");
    return 0;
}
