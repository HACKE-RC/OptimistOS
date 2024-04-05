%macro PUSHALL 0
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro POPALL 0
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
%endmacro

[extern isrHandler]

isrCommon:
    PUSHALL
    cld

    mov rdi, rsp
    call isrHandler
    POPALL

    add rsp, 16
    iretq

%macro isr 1
isr_%1:
%if !(%1 == 8 || (%1 >= 10 && %1 <= 14) || %1 == 17 || %1 == 21 || %1 == 29 || %1 == 30)
    push 0
%endif
    cli
    push %1
    jmp isrCommon
%endmacro

%assign i 0
%rep 256
isr i
%assign i i+1
%endrep

section .data
int_table:
%assign i 0
%rep 256
    dq isr_%+i
%assign i i+1
%endrep
[GLOBAL int_table]
