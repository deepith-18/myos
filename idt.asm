[bits 32]

; Load IDT into CPU
global idt_load
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; Macro to create ISR without error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common
%endmacro

; Macro to create ISR with error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp isr_common
%endmacro

; Define our exception handlers
ISR_NOERRCODE 0   ; divide by zero
ISR_NOERRCODE 1   ; debug

; Common handler — saves registers, calls C function
extern isr_handler
isr_common:
    pusha               ; save all registers

    mov ax, 0x10        ; load data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [esp+36] ; push interrupt number
    call isr_handler    ; call C handler
    add esp, 4

    popa                ; restore all registers
    add esp, 8          ; clean up stack
    sti                 ; re-enable interrupts
    iret                ; return from interrupt
