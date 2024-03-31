#include "isr.hpp"
#include "idt.hpp"
#include "../printutils/e9print.h"
#include "../acpi/ioapic.hpp"

char *exception_messages[] = {
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

uint8_t pitTicks = 0;

void isrInstall(){
    setIDTGate(0, (uint64_t)isr0);
    setIDTGate(1, (uint64_t)isr1);
    setIDTGate(2, (uint64_t)isr2);
    setIDTGate(3, (uint64_t)isr3);
    setIDTGate(4, (uint64_t)isr4);
    setIDTGate(5, (uint64_t)isr5);
    setIDTGate(6, (uint64_t)isr6);
    setIDTGate(7, (uint64_t)isr7);
    setIDTGate(8, (uint64_t)isr8);
    setIDTGate(9, (uint64_t)isr9);
    setIDTGate(10, (uint64_t)isr10);
    setIDTGate(11, (uint64_t)isr11);
    setIDTGate(12, (uint64_t)isr12);
    setIDTGate(13, (uint64_t)isr13);
    setIDTGate(14, (uint64_t)isr14);
    setIDTGate(15, (uint64_t)isr15);
    setIDTGate(16, (uint64_t)isr16);
    setIDTGate(17, (uint64_t)isr17);
    setIDTGate(18, (uint64_t)isr18);
    setIDTGate(19, (uint64_t)isr19);
    setIDTGate(20, (uint64_t)isr20);
    setIDTGate(21, (uint64_t)isr21);
    setIDTGate(22, (uint64_t)isr22);
    setIDTGate(23, (uint64_t)isr23);
    setIDTGate(24, (uint64_t)isr24);
    setIDTGate(25, (uint64_t)isr25);
    setIDTGate(26, (uint64_t)isr26);
    setIDTGate(27, (uint64_t)isr27);
    setIDTGate(28, (uint64_t)isr28);
    setIDTGate(29, (uint64_t)isr29);
    setIDTGate(30, (uint64_t)isr30);
    setIDTGate(31, (uint64_t)isr31);

    if (!wasInit){
        idtInit();
    }
    else{
        idtInitAgain();
    }

    __asm__ volatile("sti");
}

void isrHandler(uint64_t rsp){
    cpuRegisters *regs = (cpuRegisters*)rsp;
    e9_printf("\nfucked up\n");
    e9_printf("\nRIP: %d\n", regs->rip);
    e9_printf("interrupt no. : %d", regs->int_no);
    asm("hlt");
}

void isr8()
{
    e9_printf("interrupt 8");
}

void isr9(){
    e9_printf("interrupt 9");
}
void isr6(){
    e9_printf("invalid opcode!");
    asm volatile("hlt");
}
void isr13(){
    e9_printf("general protection fault!\n");
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

void pitHandler(){
    e9_printf("incrementing count!");
    ++pitTicks;
    outb(0x20, 0x20);
    writeEOI();
    idtInit();
    //    outb(0xA0, 0x20);

}