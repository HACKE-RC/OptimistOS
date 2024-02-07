#include "kernelEntry.hpp"

static BasicRenderer renderer = BasicRenderer(nullptr, nullptr);

int setupOptimist(){
    bootInfo bootInformation = getBootInfo();
    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Clear(col::black, true);
    GlobalRenderer->Print("this is working!\n");
    initGDT();
    isrInstall(renderer);
    GlobalRenderer->Print("Memory Information: \n");
    e9_printf("memory addr: %x", bootInformation.memory.freeMemStart);
//    initPaging();

//  root / eXtended system descriptor table
    uintptr_t SDT = initACPI();

    if (SDT != 0){
        e9_printf("ACPI initialized!\n");
        e9_printf("SDT addr: %x", SDT);
    }
    else{
        haltAndCatchFire(__FILE__, __LINE__);
    }

    initMADT();
    initLAPIC();

    e9_printf("\nEntry completed successfully!\n");
    return 0;
}
