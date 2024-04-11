#ifndef OPTIMISTOS_KERNELENTRY_HPP
#define OPTIMISTOS_KERNELENTRY_HPP

#include "renderer/BasicRenderer.hpp"
#include "boot.h"
#include "gdt/gdt.hpp"
#include "idt/idt.hpp"
#include "idt/isr.hpp"
#include "printutils/e9print.h"
#include "paging/pageFrameAllocator.hpp"
#include "paging/paging.hpp"
#include "acpi/acpi.hpp"
#include "acpi/madt.hpp"
#include "acpi/lapic.hpp"
#include "acpi/ioapic.hpp"
#include "smp/smp.hpp"
#include "io/io.hpp"
#include "process/process.hpp"

int setupOptimist();
extern BasicRenderer renderer;

// PIC stuff
#define PIC1         0x20		/* IO base address for master PIC */
#define PIC2         0xA0		/* IO base address for slave PIC */
#define PIC1_CMD     PIC1
#define PIC1_DAT 	 (PIC1+1)
#define PIC2_CMD     PIC2
#define PIC2_DAT 	 (PIC2+1)
extern void printNumbers();
extern void printNumbers2();

#endif //OPTIMISTOS_KERNELENTRY_HPP