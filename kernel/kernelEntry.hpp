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

int setupOptimist();

#endif //OPTIMISTOS_KERNELENTRY_HPP