#include "kernelEntry.hpp"

BasicRenderer renderer = BasicRenderer(nullptr, nullptr);

int setupOptimist(){
    bootInfo bootInformation = getBootInfo();

    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Clear(col::black, true);
    GlobalRenderer->Print("this is working!\n");
    initGDT();

    isrInstall();
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

//    initMADT();
//    initLAPIC();
//    initIOAPIC();
    initSMP();
    GlobalRenderer->Print("smp done: \n");
//  disabling PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    e9_printf("\nLAPIC ID: %d", lapicGetID());
    e9_printf("\nEntry completed successfully!\n");
    GlobalRenderer->Print(" before prco done: \n");
//    createProcessFromRoutine(idle, PRIORITY_HIGH, 0, THREAD_READY, false);
    GlobalRenderer->Print("after proc done: \n");
    pitInit(25);
    GlobalRenderer->Print("after pit done: \n");

    for (int i = 0; i < 10; i++){
        ;;
    }

    e9_printf("\ncount: %d\n", getPITCount());

    return 0;
}

void print(char* string){
    bootInfo bootInformation = getBootInfo();

    renderer = BasicRenderer(&bootInformation.framebuffer, bootInformation.psf1Font);
    GlobalRenderer = &renderer;
    GlobalRenderer->Print(string);
}