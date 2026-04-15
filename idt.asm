[bits 32]

; Load IDT into CPU
global idt_load
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; Macro for CPU exceptions (no error code)
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common
%endmacro

; Macro for CPU exceptions (with error code)
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp isr_common
%endmacro

; Macro for hardware IRQs
%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common
%endmacro

; CPU exception handlers
ISR_NOERRCODE 0
ISR_NOERRCODE 1

; Hardware IRQ handlers
IRQ 0, 32
IRQ 1, 33

; Common CPU exception handler
extern isr_handler
isr_common:
    pusha
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword [esp+32]
    call isr_handler
    add esp, 4
    popa
    add esp, 8
    sti
    iret

; Common IRQ handler
extern irq_handler
irq_common:
    pusha
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword [esp+32]
    call irq_handler
    add esp, 4
    popa
    add esp, 8
    sti
    iret
