#ifndef OPTIMISTOS_ISR_HPP
#define OPTIMISTOS_ISR_HPP
#include <cstdint>
#include "../renderer/BasicRenderer.hpp"
#include "../io/io.hpp"
#include "../printutils/e9print.h"
//#include "../acpi/ioapic.hpp"

typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t int_no;
    uint64_t err;

    uint64_t rip;
    uint64_t cs;
    uint64_t eFlags;
    uint64_t rsp;
    uint64_t ss;
} cpuRegisters;


extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern uint8_t pitTicks;

extern void isr0();
extern void isr6();
extern "C" void isr7();

extern void pitHandler();
extern void isr8();

extern void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();

extern void isr13();
extern void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void isrInstall();
extern "C" void isrHandler(uint64_t rsp);

#endif //OPTIMISTOS_ISR_HPP
