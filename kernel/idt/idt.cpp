#include "idt.hpp"

idtEntry idtEntries[256];
idtPtrStruct idtPtr;
bool wasInit = false;

#define IDT_ENTRIES 256

idtEntry idt[IDT_ENTRIES];
idtPtrStruct idt_p;
void *irq_handlers[16];

extern uint64_t isr_tbl[];

void idtInit(){
    __asm__ volatile ("sti");
    idtPtr.limit = ((uint16_t)sizeof(idtEntry) * 256) - 1;
    idtPtr.base = (uint64_t)&idtEntries;

    __asm__ volatile("lidt (%0)"
                    :
                    : "r"(&idtPtr));
    wasInit = true;
    __asm__ volatile ("cli");
}
#include "isr.hpp"
void excp_handler(cpuRegisters frame)
{
    // frame.vector);

    if (frame.int_no < 0x20)
    {
        e9_printf("\n%d: %d\n", exception_messages[frame.int_no], frame);
    }
    else if (frame.int_no >= 0x20 && frame.int_no <= 0x2f)
    {

        int irq = frame.int_no - 0x20;
        typedef void (*handler_func_t)(cpuRegisters *);

        handler_func_t handler = (handler_func_t)irq_handlers[irq];

        if (handler != nullptr)
        {
            handler(&frame);
        }

        i8259_SendEndOfInterrupt(irq);
    }
    else if (frame.int_no == 0x80)
    {
    }
}
void i8259_SendEndOfInterrupt(int irq) {
    if (irq >= 8)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}
void irq_register(uint8_t irq, void *handler)
{
    irq_handlers[irq] = handler;
}

void irq_deregister(uint8_t irq)
{
    irq_handlers[irq] = nullptr;
}

void idtInitAgain(){
    __asm__ volatile ("sti");
    __asm__ volatile("lidt (%0)"
            :
            : "r"(&idtPtr));
    __asm__ volatile ("cli");
}

void setIDTGate(uint8_t gateNumber, uintptr_t gateAddr){
    uint16_t offsetLow = (uint64_t)(gateAddr & 0xFFFF);
    uint16_t offsetMiddle = (uint64_t)((gateAddr >> 16 ) & 0xFFFF);
    uint32_t offsetHigh = (uint64_t)((gateAddr >> 32) & 0xFFFFFFFF);

    idtEntry entry = {
        .offsetLow = offsetLow,
        .segmentSelector = 0x28,
        .reserved = 0,
        .attributes = (uint8_t)0x8E,
        .offsetMiddle = offsetMiddle,
        .offsetHigh = offsetHigh,
        .reserved2 = 0
    };
    irq_register(gateNumber, (void*)gateAddr);
    idtEntries[gateNumber] = entry;
}