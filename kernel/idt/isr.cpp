#include "isr.hpp"

char *exception_messages[32] = {
        "Division by Zero",
        "Debug",
        "Non-Maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bat TSS",
        "Segment not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

uint64_t pitTicks = 0;

void isrInstall(){
    int_table[32] = (void*)pitHandler;
    asm volatile("cli");


    handlers[0xd] = isr13;
    for (int i = 0; i < 256; i++)
    {
        setIDTGate(i, (uintptr_t)int_table[i]);
    }
    if (!wasInit){
        idtInit();
    }
    else{
        idtInitAgain();
    }

    __asm__ volatile("sti");
}

extern "C" void isrHandler(uint64_t rsp){
    auto *regs = (cpuRegs*)rsp;
    if (regs->int_no < 32){
        e9_printf("Exception: %s\n", exception_messages[regs->int_no]);
        e9_printf("RIP: %x\n", regs->rip);
        reportError();
    }
    else{
    }
    {
        if (handlers[regs->int_no] != nullptr){
            (handlers[regs->int_no])(regs);
        }
    }

    if (regs->int_no >= 0x20){
        outb(0x20, 0x20);
    }
}

extern "C" void isr8()
{
    e9_printf("interrupt 8");
}

void isr9(){ e9_printf("interrupt 9"); }
void isr6(){
    e9_printf("invalid opcode!");
    asm volatile("hlt");
}
extern "C" void isr13(cpuRegs* reg){
    e9_printf("general protection fault!\n");
    e9_printf("RIP %x\n", reg->rip);
    asm volatile("hlt");
}

void isr14(){
    e9_printf("page fault!\n");
    asm volatile("hlt");
}

void isr0(){
    e9_printf("divide by zero");
    asm volatile("hlt");
}

uint32_t lockx = 0;

void pitHandler(cpuRegs* regs){
    lock(lockx);
    ++pitTicks;

    runThread(regs);

    unlock(lockx);
}