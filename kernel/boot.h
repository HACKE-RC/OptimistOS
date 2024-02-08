#pragma once
#ifndef boot_h
#define boot_h

#include <cstdint>
#include <cstddef>
#include "../limine/limine.h"
#include "printutils/e9print.h"
#define reportError() haltAndCatchFire(__FILE__, __LINE__);

struct Framebuffer
{
    void *BaseAddress;
    size_t BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelsPerScanLine;
};

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

struct PSF1_HEADER
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
};

struct PSF1_FONT
{
    PSF1_HEADER *psf1_Header;
    void *glyphBuffer;
};

struct Memory
{
    void *freeMemStart;
    void *extraMemStart;
    uint64_t freeMemSize;
    void *kernelStart;
    uint64_t kernelSize;
    void *kernelStartV;
    uint64_t hhdmOffset;
};

typedef struct {
   Framebuffer framebuffer;
   PSF1_FONT *psf1Font;
   Memory memory;
} bootInfo;

static void haltAndCatchFire(const char* fileName, uint32_t lineNo, const char* msg = ""){
    e9_printf("\nERROR at %s:%d\n", fileName, lineNo);
    e9_printf("Halting and catching fire!!\n");
    if (msg != ""){
        e9_printf("Error message: %s\n", msg);
    }
    asm volatile("hlt");
}

extern volatile struct limine_kernel_address_request kernelMemoryRequest;
extern volatile limine_kernel_file_request kernelFileRequest;
extern volatile limine_memmap_request memmapRequest;
extern volatile limine_hhdm_request hhdmRequest;
extern volatile limine_rsdp_request rsdpRequest;
extern volatile limine_smp_request smpRequest;


void setBootInfo(bootInfo bootInfo);
bootInfo getBootInfo();
#endif