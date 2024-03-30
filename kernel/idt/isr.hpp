#ifndef OPTIMISTOS_ISR_HPP
#define OPTIMISTOS_ISR_HPP
#include <cstdint>
#include "../renderer/BasicRenderer.hpp"
#include "../printutils/e9print.h"

typedef struct{
    uint64_t rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t int_no, error_code;
    uint64_t rip, cs, ds , eFlags, userEsp, ss;
} __attribute__((packed)) registers;

extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
static uint8_t pitTicks = 0;

extern void isr0();
extern void isr6();
extern "C" void isr7();

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

extern "C" void isrInstall(BasicRenderer renderer);
extern "C" void isrHandler(registers regs);

#endif //OPTIMISTOS_ISR_HPP
