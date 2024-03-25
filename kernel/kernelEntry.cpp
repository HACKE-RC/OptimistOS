#include "kernelEntry.hpp"

BasicRenderer renderer = BasicRenderer(nullptr, nullptr);

int setupOptimist(){
    bootInfo bootInformation = getBootInfo();

    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Clear(col::black, true);
    GlobalRenderer->Print("this is working!\n");
    initGDT();
    isrInstall(renderer);
    GlobalRenderer->Print("Memory Information: \n");
    e9_printf("init paging\n");
    e9_printf("memory addr: %x", bootInformation.memory.freeMemStart);
    initPaging();
    //    writeCrReg(3, PML4);
    e9_printf("cr3 write complete: %x", readCr3());
    e9_printf("\nPAGING IS WORKING\n");
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
    initIOAPIC();
    initSMP();
//  disabling PIC
    outb(PIC1_DAT, 0xFF);
    outb(PIC2_DAT, 0xFF);

    e9_printf("\nLAPIC ID: %d", lapicGetID());
    e9_printf("\nEntry completed successfully!\n");
    return 0;
}
