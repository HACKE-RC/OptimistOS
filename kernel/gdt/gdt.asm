extern gdtPtr
global gdtFlush

gdtFlush:
    lgdt [rel gdtPtr]         ; Load the GDT
    push 8
    lea rax, [rel .flush]
    push rax
    retfq
.flush
    mov eax, 0x10       ; Load the data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

   ; pop rdi            ; Pop the return address
   ; mov rax, 0x08      ; Load the code segment selector
   ; push rax
   ; push rdi
   ; retfq              ; Return to user mode

