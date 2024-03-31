#ifndef PAGING_CPP_IO_H
#define PAGING_CPP_IO_H
#include <stdint.h>

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);
extern void outw(uint16_t port, uint16_t val);
extern uint16_t inw(uint16_t port);
extern void outl(uint16_t port, uint32_t val);
extern uint32_t inl(uint16_t port);
extern void ioWait();

#endif
