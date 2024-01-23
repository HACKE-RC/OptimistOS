#pragma once
#ifndef boot_h
#define boot_h

#include <stdint.h>
#include <stddef.h>

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

struct RSDP1
{
    unsigned char Signature[8];
    uint8_t Checksum;
    uint8_t OEM_ID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
} __attribute__((packed));

struct RSDP2
{
    RSDP1 firstPart;

    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];

} __attribute__((packed));

typedef struct {
   Framebuffer framebuffer;
   PSF1_FONT *psf1Font;
   Memory memory;
} bootInfo;


void setBootInfo(bootInfo bootInfo);
bootInfo getBootInfo();
#endif