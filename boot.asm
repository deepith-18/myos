[org 0x7C00]

; Print 'B' - bootloader started
mov ah, 0x0E
mov al, 'B'
int 0x10

; Load kernel from disk
mov bx, 0x1000
mov ah, 0x02
mov al, 128
mov ch, 0
mov cl, 2
mov dh, 0
int 0x13

; Switch to 32-bit protected mode
cli                     ; disable interrupts
lgdt [gdt_descriptor]   ; load our GDT table

mov eax, cr0
or eax, 1
mov cr0, eax            ; set protected mode bit

jmp 0x08:protected_mode ; far jump into 32-bit

; ── GDT Table ──────────────────────────
gdt_start:
    dq 0                        ; null descriptor

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ── 32-bit code starts here ────────────
[bits 32]
protected_mode:
    mov ax, 0x10        ; data segment
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000    ; set up stack

    jmp 0x1000          ; jump to kernel

times 510-($-$$) db 0
dw 0xAA55
