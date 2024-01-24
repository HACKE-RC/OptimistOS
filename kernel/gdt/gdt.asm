global gdtFlush

gdtFlush:
    lgdt [rdi]         ; Load the GDT
    mov ax, 0x10       ; Load the data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi            ; Pop the return address
    mov rax, 0x08      ; Load the code segment selector
    push rax
    push rdi
    retfq              ; Return to user mode

