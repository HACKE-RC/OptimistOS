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

    initMADT();
    initLAPIC();
    initIOAPIC();
    initSMP();
    GlobalRenderer->Print("smp done: \n");
//  disable PIC
//    outb(PIC1_DAT, 0xff);
//    outb(PIC2_DAT, 0xff);

// keeping it on'
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
    createProcessFromRoutine((uintptr_t)idle, PRIORITY_HIGH, 0, THREAD_READY, true);
    createProcessFromRoutine((uintptr_t)printNumbers, PRIORITY_HIGH, 0, THREAD_READY, true);
    GlobalRenderer->Print("after proc done: \n");
    pitInit(25);
    GlobalRenderer->Print("after pit done: \n");

    e9_printf("\ncount: %d\n", getPITCount());
    return 0;
}

void printNumbers(){
    for (int i = 0; i <= 400000; i++){
        if (i * 2 == 400){
            e9_printf("%d: 200\n", i);
        }
        else if (i * 2 == 600){
            e9_printf("%d: 300\n", i);
        }
        else if (i * 2 == 4000){
            e9_printf("%d: 2000\n", i);
        }
    }
    e9_printf("done");
}