#ifndef OPTIMISTOS_ISR_HPP
#define OPTIMISTOS_ISR_HPP
#include <stdint.h>
#include "../renderer/BasicRenderer.hpp"

typedef struct{
    uint64_t ds;
    uint64_t rdi, rsi, rbp, rsp, rdx, rcx, rbx, rax;
    uint64_t int_no, error_code;
    uint64_t rip, cs, eFlags, userEsp, ss;
} registers;

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
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
