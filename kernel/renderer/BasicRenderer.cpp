#include "BasicRenderer.hpp"

BasicRenderer::BasicRenderer(Framebuffer *framebuffer, PSF1_FONT *psf1_font)
{
    color = 0xffffffff;
    CursorPosition = {0, 0};
    this->framebuffer = framebuffer;
    this->psf1_font = psf1_font;
}

void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;
    char *fontPtr = (char *)psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);

    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
            {
                *(unsigned int *)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void BasicRenderer::Print(const char *str)
{
    char *chr = (char *)str;
    while (*chr != 0)
    {
        switch (*chr)
        {
            case '\n':
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
                break;

            case '\t':
                CursorPosition.X += 8;
                break;
            
            default:
                PutChar(*chr, CursorPosition.X, CursorPosition.Y);
                CursorPosition.X += 8;
                break;
        }

        if (CursorPosition.X + 8 > framebuffer->Width)
        {
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }

        chr++;
    }
}

//#include <limits.h>  // for LONG_MIN and LONG_MAX
unsigned long long int LONG_MIN =  -9223372036854775808;
unsigned long long int LONG_MAX = 9223372036854775807;

void BasicRenderer::PrintInt(int num)
{
    // Handle the case when the number is 0
    if (num == 0)
    {
        PutChar('0', CursorPosition.X, CursorPosition.Y);
        CursorPosition.X += 8;
        return;
    }

    // Handle negative numbers
    if (num < 0)
    {
        PutChar('-', CursorPosition.X, CursorPosition.Y);
        CursorPosition.X += 8;
        // Avoid overflow when num is LONG_MIN
        if (num == LONG_MIN)
        {
            PutChar('8', CursorPosition.X, CursorPosition.Y);
            CursorPosition.X += 8;
            num /= -10;
        }
        num = -num;
    }

    // Temporary buffer to store digits in reverse order
    char buffer[21];  // 21 is enough for 64-bit integers
    int bufferIndex = 0;

    // Extract digits in reverse order
    while (num > 0)
    {
        buffer[bufferIndex++] = '0' + (num % 10);
        num /= 10;
    }

    // Print digits in correct order
    for (int i = bufferIndex - 1; i >= 0; --i)
    {
        PutChar(buffer[i], CursorPosition.X, CursorPosition.Y);
        CursorPosition.X += 8;
    }
}


void BasicRenderer::Clear(uint32_t color, bool resetCursor)
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y++)
    {
        for (int64_t x = 0; x < framebuffer->Width; x++)
        {
            *((uint32_t *)(fbBase + 4 * (x + pxlsPerScanline * y))) = color;
        }
    }

    if (resetCursor)
    {
        CursorPosition = {0, 0};
    }
}
