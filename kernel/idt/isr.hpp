#ifndef OPTIMISTOS_ISR_HPP
#define OPTIMISTOS_ISR_HPP
#include <cstdint>
#include "../renderer/BasicRenderer.hpp"
#include "../printutils/e9print.h"
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

static void isr6(){
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    e9_printf("invalid opcode!");
    asm volatile("hlt");
}
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
static void isr13(){
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    e9_printf("general protection fault!");
    asm volatile("hlt");
}

static void isr14(){
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    e9_printf("page fault!");
    asm volatile("hlt");
}

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
