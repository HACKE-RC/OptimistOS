#include "boot.h"
#include "../limine/limine.h"
#include "kernelEntry.hpp"
#include <cstddef>
#include <cstdint>

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static volatile struct limine_terminal_request terminal_request = {
        .id = LIMINE_TERMINAL_REQUEST, .revision = 0};

static volatile struct limine_framebuffer_request framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

volatile struct limine_rsdp_request rsdpRequest = {
        .id = LIMINE_RSDP_REQUEST,
        .revision = 0
};

volatile struct limine_smp_request smpRequest{
        .id = LIMINE_SMP_REQUEST,
        .revision = 0
};


static volatile limine_module_request module_request = {
        .id = LIMINE_MODULE_REQUEST, .revision = 0};

volatile limine_hhdm_request hhdmRequest = {.id = LIMINE_HHDM_REQUEST,
        .revision = 0};

volatile limine_kernel_address_request kernelMemoryRequest = {
        .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0, .response = nullptr};

volatile limine_kernel_file_request kernelFileRequest{
        .id = LIMINE_KERNEL_FILE_REQUEST, .revision = 0, .response = nullptr};

volatile limine_memmap_request memmapRequest = {
        .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr};

// #define LIMINE_5_LEVEL_PAGING_REQUEST { LIMINE_COMMON_MAGIC,
// 0x94469551da9b3192, 0xebe5e86db7382888}

// static volatile limine_5_level_paging_request paging_request = {
//     .id = LIMINE_5_LEVEL_PAGING_REQUEST,
//     .revision = 0
// };

// #define LIMINE_ENTRY_POINT_REQUEST { LIMINE_COMMON_MAGIC, 0x13d86c035a1cd3e1,
// 0x2b0caa89d8f3026a }

// extern "C" void _start(void);

// static volatile limine_entry_point_request entry_request = {
//     .id = LIMINE_ENTRY_POINT_REQUEST,
//     .revision = 0,
//     .entry = _start
// };

// this file is our kernel
// this request allows us to access the address of our kernel

static void done() {
    e9_printf("initialisation done");
    for (;;) {
        asm volatile("hlt");
    }
}

#include "printutils/e9print.h"

static void print_file(struct limine_file *file) {
    e9_printf("File->Revision: %d", file->revision);
    e9_printf("File->Address: %x", file->address);
    e9_printf("File->Size: %x", file->size);
    e9_printf("File->Path: %s", file->path);
    e9_printf("File->CmdLine: %s", file->cmdline);
    e9_printf("File->MediaType: %d", file->media_type);
    e9_printf("File->PartIndex: %d", file->partition_index);
    e9_printf("File->TFTPIP: %d.%d.%d.%d", (file->tftp_ip & (0xff << 0)) >> 0,
              (file->tftp_ip & (0xff << 8)) >> 8,
              (file->tftp_ip & (0xff << 16)) >> 16,
              (file->tftp_ip & (0xff << 24)) >> 24);
    e9_printf("File->TFTPPort: %d", file->tftp_port);
    e9_printf("File->MBRDiskId: %x", file->mbr_disk_id);
    e9_printf("File->GPTDiskUUID: %x-%x-%x-%x", file->gpt_disk_uuid.a,
              file->gpt_disk_uuid.b, file->gpt_disk_uuid.c,
              *(uint64_t *)file->gpt_disk_uuid.d);
    e9_printf("File->GPTPartUUID: %x-%x-%x-%x", file->gpt_part_uuid.a,
              file->gpt_part_uuid.b, file->gpt_part_uuid.c,
              *(uint64_t *)file->gpt_part_uuid.d);
    e9_printf("File->PartUUID: %x-%x-%x-%x", file->part_uuid.a, file->part_uuid.b,
              file->part_uuid.c, *(uint64_t *)file->part_uuid.d);
}

static const char *get_memmap_type(uint64_t type) {
    switch (type) {
        case LIMINE_MEMMAP_USABLE:
            return "Usable";
        case LIMINE_MEMMAP_RESERVED:
            return "Reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI reclaimable";
        case LIMINE_MEMMAP_ACPI_NVS:
            return "ACPI NVS";
        case LIMINE_MEMMAP_BAD_MEMORY:
            return "Bad memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader reclaimable";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            return "Kernel and modules";
        case LIMINE_MEMMAP_FRAMEBUFFER:
            return "Framebuffer";
        default:
            return "???";
    }
}

static void write_shim(const char *s, uint64_t l) {
    struct limine_terminal *terminal = terminal_request.response->terminals[0];

    terminal_request.response->write(terminal, s, l);
}

static void *startRAMAddr = nullptr;
static void *quickMalloc(uint64_t size) {
    if (startRAMAddr == nullptr) {
        e9_printf("LIMINE_ERROR: Cannot malloc from NULL!");
        done();
    }
    void *temp = startRAMAddr;
    startRAMAddr = (void *)((uint64_t)startRAMAddr + size);
    return temp;
}

static bool checkStringEndsWith(const char *str, const char *end) {
    const char *_str = str;
    const char *_end = end;

    while (*str != 0)
        str++;
    str--;

    while (*end != 0)
        end++;
    end--;

    while (true) {
        if (*str != *end)
            return false;

        str--;
        end--;

        if (end == _end || (str == _str && end == _end))
            return true;

        if (str == _str)
            return false;
    }

    return true;
}

static limine_file *getFile() {
    limine_module_response *module_response = module_request.response;
    for (size_t i = 0; i < module_response->module_count; i++) {
        limine_file *f = module_response->modules[i];
        if (checkStringEndsWith(f->path, "zap-light16.psf"))
            return f;
    }
    return nullptr;
}

static bootInfo globalBootInfo;

void setBootInfo(bootInfo bootInfo) { globalBootInfo = bootInfo; }

bootInfo getBootInfo() { return globalBootInfo; }

// The following will be our kernel's entry point.
extern "C" void _start(void) {
    bootInfo bootInformation;
    // Ensure we got a terminal
    if (terminal_request.response == nullptr ||
        terminal_request.response->terminal_count < 1) {
        done();
    }

    limine_print = write_shim;

    // we should now be able to call the limine terminal to print out
    // a simple "hello world" to screen.
    struct limine_terminal *terminal = terminal_request.response->terminals[0];

    terminal_request.response->write(terminal, "Starting Boot init...\n\n", 24);

    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        e9_printf("LIMINE_ERROR: ");
        terminal_request.response->write(terminal, "Framebuffer is NULL!\n", 22);

        done();
    } else {
        e9_printf("> LIMINE_SUCCESS: ");
        terminal_request.response->write(terminal, "Framebuffer loaded!\n", 21);
    }

    Framebuffer fb{};
    {
        limine_framebuffer *lFb = framebuffer_request.response->framebuffers[0];
        fb.BaseAddress = lFb->address;
        fb.Width = lFb->width;
        fb.Height = lFb->height;
        fb.PixelsPerScanLine = lFb->pitch / 4;
        fb.BufferSize = lFb->height * lFb->pitch; // lFb->edid_size;
    }
    //    RSDP2 *rsdp;
    //    {
    //        rsdp = (RSDP2 *)rsdpRequest.response->address;
    //    }

    if (memmapRequest.response == NULL) {
        e9_printf("LIMINE_ERROR: ");
        e9_printf("Memory map not passed\n");

        done();
    }

    void *freeMemStart = nullptr;
    uint64_t freeMemSize = 0;

    if (kernelMemoryRequest.response == NULL) {
        e9_printf("LIMINE_ERROR: ");
        e9_printf("Kernel address not passed\n");

        done();
    }

    struct limine_kernel_address_response *kernelAddressResponse =
            kernelMemoryRequest.response;
    e9_printf("Information about where the kernel is loaded: \n");
    e9_printf("Kernel address feature, revision %d\n",
              kernelAddressResponse->revision);
    e9_printf("Physical base: %x\n", kernelAddressResponse->physical_base);
    e9_printf("Virtual base: %x\n\n", kernelAddressResponse->virtual_base);

    void *kernelStart = (void *)kernelAddressResponse->physical_base;
    void *kernelStartV = (void *)kernelAddressResponse->virtual_base;
    uint64_t kernelSize = 1;

    limine_memmap_response *memoryMapResponse = memmapRequest.response;
    for (size_t i = 0; i < memoryMapResponse->entry_count; i++) {
        limine_memmap_entry *e = memoryMapResponse->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE) {
//    use the largest free memory block
            if (e->length > freeMemSize) {
                freeMemStart = (void *)e->base;
                freeMemSize = e->length;
            }
        } else if (e->base == (uint64_t)kernelStart) {
            kernelSize = e->length;
        }
    }

    if (freeMemStart == nullptr) {
        e9_printf("No valid Memory space found for OS!\n");
        done();
    }

    startRAMAddr = freeMemStart;

    if (module_request.response == nullptr) {
        e9_printf("Modules not passed!\n\n");
        done();
    }

    PSF1_FONT font{};
    {
        const char *fName = "zap-light16.psf";
        limine_file *file = getFile();
        if (file == nullptr) {
            e9_printf("LIMINE_ERROR: ");
            e9_printf("Failed to get Font \"%s\"!\n", fName);

            done();
        }

        font.psf1_Header = (PSF1_HEADER *)file->address;
        if (font.psf1_Header->magic[0] != 0x36 ||
            font.psf1_Header->magic[1] != 0x04) {
            e9_printf("LIMINE_ERROR: ");
            e9_printf("FONT HEADER INVALID!\n");

            done();
        }

        font.glyphBuffer = (void *)((uint64_t)file->address + sizeof(PSF1_HEADER));
    }

    if (hhdmRequest.response == NULL) {
        e9_printf("HHDM request wasn't fulfilled");
        done();
    }

    Memory memory{};
    {
        memory.freeMemStart = startRAMAddr;
        memory.extraMemStart = freeMemStart;
        memory.freeMemSize = freeMemSize;
        memory.kernelStart = kernelStart;
        memory.kernelSize = kernelSize;
        memory.kernelStartV = kernelStartV;
        memory.hhdmOffset = hhdmRequest.response->offset;
    }

    e9_printf("LIMINE_INFO: ");
    e9_printf("Kernel Start: %x (Size: %d Bytes)\n", (uint64_t)kernelStart,
              kernelSize);

    e9_printf("LIMINE_INFO: ");
    e9_printf("OS has %d MB of RAM. (Starts at %x)\n\n", freeMemSize / 1000000,
              (uint64_t)freeMemStart);

    e9_printf("> LIMINE_SUCCESS: ");

    terminal_request.response->write(terminal, "Completed Boot Init!\n", 23);
    bootInformation.framebuffer = fb;
    bootInformation.psf1Font = &font;
    bootInformation.memory = memory;
    setBootInfo(bootInformation);
    setupOptimist();
    idle();
}

void idle(){
//    e9_printf("idle hu bhai\n");
    while (true){
        ;;
    }
}
